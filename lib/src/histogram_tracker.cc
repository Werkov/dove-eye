#include "dove_eye/histogram_tracker.h"

#include <cassert>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

#define LOG_SEARCH_MAT

using cv::calcBackProject;
using cv::calcHist;
using cv::cvtColor;
using cv::mixChannels;
using cv::normalize;
using std::vector;

namespace dove_eye {


bool HistogramTracker::InitTrackerData(const cv::Mat &data, const Mark &mark) {
  assert(mark.type == Mark::kRectangle);

  cv::Rect roi(mark.top_left, mark.top_left + mark.size);

  if (roi.br().x >= data.cols ||
      roi.br().y >= data.rows) {
    return false;
  }

  auto roi_data = data(roi);


  cv::Mat hsv;
  cvtColor(roi_data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  minMaxLoc(hsv_components[1], &data_.srange[0], &data_.srange[1]);
  minMaxLoc(hsv_components[2], &data_.vrange[0], &data_.vrange[1]);

  data_.srange[0] = std::max(10.0, data_.srange[0]);

  DEBUG("sat: %f:%f\tval: %f:%f",
        data_.srange[0],
        data_.srange[1],
        data_.vrange[0],
        data_.vrange[1]);

  cv::Mat hue(hsv.size(), hsv.depth());
  
  const float *prange = data_.hrange;

  /* Calculate histogram */
  calcHist(&hsv_components[0],
           1, /* no. of images */
           nullptr, /* channels, can be nullptr when dims == no. of images */
           cv::Mat(), /* mask */
           data_.histogram,
           1, /* dims */
           &data_.histogram_size,
           &prange);

  normalize(data_.histogram, data_.histogram, 0, 255, CV_MINMAX);

#ifdef LOG_SEARCH_MAT
  log_color_hist(reinterpret_cast<size_t>(this) * 100 + 10,
                 data_.histogram, data_.histogram_size);
#endif

  data_.size = roi.size();

  return true;
}

/** Wrapper for OpenCV function CamShift
 * @see SearchingTracker::Search()
 */
bool HistogramTracker::Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result,
      double *quality) const {

  using cv::Scalar;

  const HistogramData &hist_data = static_cast<const HistogramData &>(tracker_data);
  auto extended_roi = cv::Rect(cv::Point(0, 0), data.size());
  if (roi) {
    extended_roi &= cv::Rect(cv::Point(0, 0), data.size());
  }

  // FIXME is this condition necessary?
  if (extended_roi.width < hist_data.size.width ||
      extended_roi.height < hist_data.size.height) {
    return false;
  }

  auto data_roi = data(extended_roi);
  cv::Mat hsv_mask;
  auto data_hue = PreprocessImage(data_roi, hist_data, &hsv_mask);
  const float *prange = hist_data.hrange;

  cv::Mat backproj;
  calcBackProject(&data_hue, 1,
                  0, /* channels */
                  hist_data.histogram,
                  backproj,
                  &prange);

  auto radius = parameters().Get(Parameters::TEMPLATE_RADIUS);
  cv::Size blur_size(radius, radius);

  blur_size.width += 1 - (blur_size.width % 2);
  blur_size.height += 1 - (blur_size.height % 2);

  /* sigma = 0 -> compute from size */
  cv::GaussianBlur(backproj, backproj, blur_size, 0);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 3, backproj);
  cv::threshold(backproj, backproj, 128, 255, cv::THRESH_BINARY);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 4, backproj);

  if (mask) {
    log_mat(reinterpret_cast<size_t>(this) * 100 + 5, *mask);
    cv::Mat tmp;
    mask->copyTo(tmp, backproj);
    backproj = tmp.clone();
    log_mat(reinterpret_cast<size_t>(this) * 100 + 66, backproj);

    vector<vector<cv::Point>> contours;
    cv::findContours(backproj, contours,
                     cv::noArray(), /* hierarchy */
                     CV_RETR_LIST,
                     CV_CHAIN_APPROX_SIMPLE);
    log_mat(reinterpret_cast<size_t>(this) * 100 + 68, backproj);

    cv::drawContours(data_roi, contours, -1, Scalar(255, 100, 0), 2);

    if (contours.size() > 0) {
      ContoursToResult(contours, result);
      return true;
    }
  }
  log_mat(reinterpret_cast<size_t>(this) * 100 + 67, backproj);


  
#ifdef LOG_SEARCH_MAT
  log_mat(reinterpret_cast<size_t>(this) * 100 + 1, data_hue);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 2, data_roi);

  log_mat(reinterpret_cast<size_t>(this) * 100 + 6, backproj);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 7, hsv_mask);
#endif


  return false;

  if (quality) {
    // FIXME definition of quality
    *quality = 1;
  }
  
  return true;
}

cv::Mat HistogramTracker::PreprocessImage(const cv::Mat &data,
                                          const HistogramData &hist_data,
                                          cv::Mat *mask) const {
  using cv::Scalar;
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

void HistogramTracker::ContoursToResult(
    const vector<vector<cv::Point>> contours,
    Mark *result) const {

  double max_area = 0;
  const vector<cv::Point> *best_contour;

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
  result->type = Mark::kRectangle;
  result->top_left = rect.tl();
  result->size = rect.br() - rect.tl();
}
} // namespace dove_eye
