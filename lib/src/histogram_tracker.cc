#include "dove_eye/histogram_tracker.h"

#include <cassert>

#include <opencv2/opencv.hpp>

#include "config.h"
#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

using cv::calcBackProject;
using cv::calcHist;
using cv::cvtColor;
using cv::mixChannels;
using cv::normalize;
using cv::Scalar;
using std::vector;

namespace dove_eye {

bool HistogramTracker::InitTrackerData(const cv::Mat &data, const Mark &mark) {
  assert(mark.type == Mark::kRectangle);

  /* Check the size of mark doesn't extend size of the image */
  cv::Rect roi(mark.top_left, mark.top_left + mark.size);

  if (roi.br().x >= data.cols ||
      roi.br().y >= data.rows) {
    return false;
  }

  data_.size = roi.size();

  /*
   * Calculate (H)SV ranges of selected area and then histogram of H(ue)
   * component.
   */
  auto roi_data = data(roi);

  cv::Mat hsv;
  cvtColor(roi_data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  minMaxLoc(hsv_components[1], &data_.srange[0], &data_.srange[1]);
  minMaxLoc(hsv_components[2], &data_.vrange[0], &data_.vrange[1]);

  DEBUG("sat: %f:%f\tval: %f:%f",
        data_.srange[0],
        data_.srange[1],
        data_.vrange[0],
        data_.vrange[1]);

  /* Calculate histogram */
  cv::Mat hue(hsv.size(), hsv.depth());
  const float *prange = data_.hrange;
  calcHist(&hsv_components[0],
           1, /* no. of images */
           nullptr, /* channels, can be nullptr when dims == no. of images */
           cv::Mat(), /* mask */
           data_.histogram,
           1, /* dims */
           &data_.histogram_size,
           &prange);

  normalize(data_.histogram, data_.histogram, 0, 255, CV_MINMAX);

#ifdef CONFIG_DEBUG_HIGHGUI
  log_color_hist(reinterpret_cast<size_t>(this) * 100 + 10,
                 data_.histogram, data_.histogram_size);
#endif

  return true;
}

/** Wrapper for OpenCV function calcBackProject
 * @see SearchingTracker::Search()
 */
bool HistogramTracker::Search(
      const cv::Mat &data,
      TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const {
  const HistogramData &hist_data = static_cast<const HistogramData &>(tracker_data);

  DEBUG("%s([%i, %i], [%i, %i], %p[%i, %i]@[%i, %i], %p, %f, res)",
        __func__,
        data.cols, data.rows,
        hist_data.size.width, hist_data.size.height,
        roi, (roi ? roi->width : 0), (roi ? roi->height : 0),
             (roi ? roi->x : 0),     (roi ? roi->y : 0),
        mask,
        threshold);

  auto extended_roi = cv::Rect(cv::Point(0, 0), data.size());
  if (roi) {
    extended_roi &= *roi;
  }

  if (extended_roi.area() == 0) {
    DEBUG("%s zero-area", __func__);
    return false;
  }

  auto data_roi = data(extended_roi);
  cv::Mat hsv_mask;
  auto data_hue = PreprocessImage(data_roi, hist_data, &hsv_mask);
  const float *prange = hist_data.hrange;
  log_mat(reinterpret_cast<size_t>(this) * 100 + 1, data_roi);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 2, hsv_mask);

  cv::Mat backproj;
  calcBackProject(&data_hue, 1,
                  0, /* channels */
                  hist_data.histogram,
                  backproj,
                  &prange);

  log_mat(reinterpret_cast<size_t>(this) * 100 + 3, backproj);
  /* Apply HSV mask before blurring */
  backproj &= hsv_mask;

  /* Gaussiuan blur
   * sigma = 0 -> calculated from size
   */
  auto radius = parameters().Get(Parameters::TEMPLATE_RADIUS);
  cv::Size blur_size(radius, radius);

  blur_size.width += 1 - (blur_size.width % 2);
  blur_size.height += 1 - (blur_size.height % 2);

  cv::GaussianBlur(backproj, backproj, blur_size, 0);

  /* Apply threshold */
  cv::threshold(backproj, backproj, threshold * 255, 255, cv::THRESH_BINARY);

  /* Apply (motion) mask */
  if (mask) {
    auto mask_roi = (*mask)(extended_roi);
    backproj &= mask_roi;
  }

  log_mat(reinterpret_cast<size_t>(this) * 100 + 4, backproj);

  ContourVector contours;
  cv::findContours(backproj, contours,
                   cv::noArray(), /* hierarchy */
                   CV_RETR_LIST,
                   CV_CHAIN_APPROX_SIMPLE);

#ifdef CONFIG_DEBUG_HIGHGUI
  cv::drawContours(data_roi, contours, -1, Scalar(255, 100, 0), 2);
#endif

  if (contours.size() == 0) {
    DEBUG("%s no-contours", __func__);
    return false;
  }

  ContoursToMark(contours, result);

  /* Apply ROI offset */
  result->top_left.x += extended_roi.x;
  result->top_left.y += extended_roi.y;

  return true;
}

/**
 * @param[in]   data      image to preprocess
 * @param[in]   hist_data histogram data
 * @param[out]  mask      (H)SV mask of image from histogram data
 *
 * @return  hue component of the image
 */
cv::Mat HistogramTracker::PreprocessImage(const cv::Mat &data,
                                          const HistogramData &hist_data,
                                          cv::Mat *mask) const {
  cv::Mat hsv;

  cvtColor(data, hsv, cv::COLOR_BGR2HSV);

  if (mask) {
    cv::inRange(hsv,
        Scalar(hist_data.hrange[0], hist_data.srange[0], hist_data.vrange[0]),
        Scalar(hist_data.hrange[1], hist_data.srange[1], hist_data.vrange[1]),
        *mask);
  }

  cv::Mat hue(hsv.size(), hsv.depth());
  int ch[] = {0, 0};
  mixChannels(&hsv, 1, &hue, 1, ch, 1);

  return hue;
}

void HistogramTracker::ContoursToMark(
    const ContourVector &contours,
    Mark *mark) const {

  double max_area = 0;
  const Contour *best_contour;

  for (auto &contour: contours) {
    auto area = cv::contourArea(contour);
    if (area < max_area) {
      continue;
    }
    max_area = area;
    best_contour = &contour;
  }

  assert(best_contour);
  auto rect = cv::boundingRect(*best_contour);
  mark->type = Mark::kRectangle;
  mark->top_left = rect.tl();
  mark->size = rect.br() - rect.tl();
}

} // namespace dove_eye
