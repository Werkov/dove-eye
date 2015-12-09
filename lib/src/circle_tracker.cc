#include "dove_eye/circle_tracker.h"

#include <cassert>

#include <opencv2/opencv.hpp>

#include "config.h"
#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

using cv::HoughCircles;
using cv::cvtColor;
using cv::mixChannels;
using cv::Scalar;
using std::vector;

namespace dove_eye {

bool CircleTracker::InitTrackerData(const cv::Mat &data, const Mark &mark) {
  assert(mark.type == Mark::kCircle);
  const double inner = 0.707; /* sqrt(0.5) is relative size of inscribed square */

  /* Take data from the inner part of the circle only */
  Point2 rad(mark.radius, mark.radius);
  cv::Rect roi(mark.center - rad * inner, mark.center + rad * inner);
 
  data_.radius = mark.radius;

  /* Ensure the size of mark doesn't extend size of the image */
  cv::Rect boundary(0, 0, data.cols, data.rows);
  cv::Rect safe_roi = roi & boundary;

  /*
   * Calculate HSV ranges of selected area for thresholding.
   */
  auto roi_data = data(safe_roi);

  log_mat(reinterpret_cast<size_t>(this) * 100 + 4, roi_data);

  cv::Mat hsv;
  cvtColor(roi_data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  minMaxLoc(hsv_components[0], &data_.hrange[0], &data_.hrange[1]);
  minMaxLoc(hsv_components[1], &data_.srange[0], &data_.srange[1]);
  minMaxLoc(hsv_components[2], &data_.vrange[0], &data_.vrange[1]);

  DEBUG("hue: %f:%f\tsat: %f:%f\tval: %f:%f",
        data_.hrange[0],
        data_.hrange[1],
        data_.srange[0],
        data_.srange[1],
        data_.vrange[0],
        data_.vrange[1]);

  return true;
}

/** Use Hough transform to find best matching circle
 *
 * @see SearchingTracker::Search()
 */
bool CircleTracker::Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const {
  const CircleData &circle_data = static_cast<const CircleData &>(tracker_data);

  auto extended_roi = cv::Rect(cv::Point(0, 0), data.size());
  if (roi) {
    extended_roi &= *roi;
  }

  if (extended_roi.area() == 0) {
    DEBUG("%s zero-area", __func__);
    return false;
  }

  auto data_roi = data(extended_roi);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 1, data_roi);
  auto data_proc = PreprocessImage(data_roi, circle_data, threshold);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 2, data_proc);

  CircleVector circles;
  HoughCircles(data_proc, circles, CV_HOUGH_GRADIENT,
               2, // accumulator ratio (to original image resolution)
               data_proc.rows / 2, //minDist between centers of circles
               100, //param1 (Canny threshold)
               50, //param2 (accumulator threshold)
               2, //minRadius
               data_proc.rows / 2); // maxRadius


  /* (Motion) mask is ignored. */

#ifdef CONFIG_DEBUG_HIGHGUI
  auto circ_mat = data_roi.clone();
  for (auto circle : circles) {
    Point2 center(circle[0], circle[1]);
    double radius(circle[2]);
    cv::circle(circ_mat, center, radius, cv::Scalar(255, 100, 0), 2);
  }
  log_mat(reinterpret_cast<size_t>(this) * 100 + 3, circ_mat);
#endif

  if (!CirclesToMark(circles, result)) {
    DEBUG("%s no-circles", __func__);
    return false;
  }

  /* Apply ROI offset */
  result->center.x += extended_roi.tl().x;
  result->center.y += extended_roi.tl().y;

  return true;
}

/**
 * @param[in]   data      image to preprocess
 * @param[in]   circle_data
 * @param[in]   threshold HSV threshoold
 *
 * @return  hue component of the image
 */
cv::Mat CircleTracker::PreprocessImage(const cv::Mat &data,
                                       const CircleData &circle_data,
                                       const double threshold) const {
  cv::Mat hsv;
  cv::Mat mask;

  cvtColor(data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);
  /* Apply threshold */
  cv::inRange(hsv_components[0], circle_data.hrange[0], circle_data.hrange[1], mask);

  /* Denoise */
  const auto radius = parameters().Get(Parameters::TEMPLATE_RADIUS);
  cv::Size blur_size(radius, radius);

  blur_size.width += 1 - (blur_size.width % 2);
  blur_size.height += 1 - (blur_size.height % 2);

  log_mat(reinterpret_cast<size_t>(this) * 100 + 11, mask);
  cv::GaussianBlur(mask, mask, blur_size, 0);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 12, mask);
  return mask;

  //TODO use threshold to broaden range

  /* Find edges */
  cv::Mat edges;
  cv::Canny(mask, edges, 1, 3);

  return edges;
}

bool CircleTracker::CirclesToMark(
    const CircleVector &circles,
    Mark *mark) const {

  if (circles.size() == 0) {
    return false;
  }

  /* Most probable circles should be sorted first, so use the first one. */
  auto circle = circles[0];
  mark->type = Mark::kCircle;
  mark->center = Point2(circle[0], circle[1]);
  mark->radius = circle[2];

  return true;
}

} // namespace dove_eye
