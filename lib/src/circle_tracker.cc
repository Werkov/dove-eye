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

  /* Check the size of mark doesn't extend size of the image */
  Point2 rad(mark.radius, mark.radius);
  cv::Rect roi(mark.center - rad, mark.center + rad);

  if (roi.br().x >= data.cols ||
      roi.br().y >= data.rows) {
    return false;
  }

  /*
   * Calculate HSV ranges of selected area for thresholding.
   */
  auto roi_data = data(roi);

  cv::Mat hsv;
  cvtColor(roi_data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  minMaxLoc(hsv_components[2], &data_.hrange[0], &data_.hrange[1]);
  minMaxLoc(hsv_components[1], &data_.srange[0], &data_.srange[1]);
  minMaxLoc(hsv_components[2], &data_.vrange[0], &data_.vrange[1]);

  DEBUG("hue: %f:%f\tsat: %f:%f\tval: %f:%f",
        data_.hrange[0],
        data_.hrange[1],
        data_.srange[0],
        data_.srange[1],
        data_.vrange[0],
        data_.vrange[1]);

  data_.radius = mark.radius;
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
  auto data_gray = PreprocessImage(data_roi, circle_data, threshold);

  CircleVector circles;
  HoughCircles(data_gray, circles, CV_HOUGH_GRADIENT,
               2, // accumulator ratio
               data_gray.rows / 4, //minDist
               100, //param1
               100, //param2
               0, //minRadius
               0); // maxRadius


  /* (Motion) mask is ignored. */

#ifdef CONFIG_DEBUG_HIGHGUI
  //cv::drawContours(data_roi, contours, -1, Scalar(255, 100, 0), 2);
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
  cv::Mat result;

  cvtColor(data, hsv, cv::COLOR_BGR2HSV);

  //TODO use threshold to broaden range
  cv::inRange(hsv,
      Scalar(circle_data.hrange[0], circle_data.srange[0], circle_data.vrange[0]),
      Scalar(circle_data.hrange[1], circle_data.srange[1], circle_data.vrange[1]),
      result);

  return result;
}

bool CircleTracker::CirclesToMark(
    const CircleVector &circles,
    Mark *mark) const {

  if (circles.size() != 1) {
    return false;
  }

  auto circle = circles[0];
  mark->type = Mark::kCircle;
  mark->center = Point2(circle[0], circle[1]);
  mark->radius = circle[2];

  return mark;
}

} // namespace dove_eye
