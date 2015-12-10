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
  return UpdateData(data_, data, mark);
}

/** Use Hough transform to find best matching circle
 *
 * @see SearchingTracker::Search()
 */
bool CircleTracker::Search(
      const cv::Mat &data,
      TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const {
  CircleData &circle_data = static_cast<CircleData &>(tracker_data);
  const double radius_factor = 1.5;

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
               10, //param1 (Canny threshold)
               25, //param2 (accumulator threshold)
               circle_data.radius / radius_factor, //minRadius
               circle_data.radius * radius_factor); // maxRadius


  /* (Motion) mask is ignored. */

  auto score = CirclesToMark(data_proc, circles, result);
  DEBUG("%s score: %f", __func__, score);
  
  /* Apply ROI offset */
  result->center.x += extended_roi.tl().x;
  result->center.y += extended_roi.tl().y;

  if (score < 1e-9) {
    DEBUG("%s no-circles", __func__);
    return false;
  } else if (score > 0.4) {
    UpdateData(circle_data, data, *result);
  }

  return true;
}

bool CircleTracker::UpdateData(CircleData &circle_data, const cv::Mat &data,
                  const Mark &mark) const {
  /* Take data from the inner part of the circle only */
  Point2 rad(mark.radius, mark.radius);
  cv::Rect roi(mark.center - rad, mark.center + rad);
 
  /* Ensure the size of mark doesn't extend size of the image */
  cv::Rect boundary(0, 0, data.cols, data.rows);
  cv::Rect safe_roi = roi & boundary;

  /* Calculate histogram of hue values */
  cv::Mat hsv;
  cvtColor(data(safe_roi), hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  minMaxLoc(hsv_components[1], &circle_data.srange[0], &circle_data.srange[1]);
  minMaxLoc(hsv_components[2], &circle_data.vrange[0], &circle_data.vrange[1]);

  DEBUG("sat: %f:%f\tval: %f:%f",
        circle_data.srange[0],
        circle_data.srange[1],
        circle_data.vrange[0],
        circle_data.vrange[1]);

  cv::Mat hue(hsv.size(), hsv.depth());
  const float *prange = circle_data.hrange;
  cv::calcHist(&hsv_components[0],
           1, /* no. of images */
           nullptr, /* channels, can be nullptr when dims == no. of images */
           cv::Mat(), /* mask */
           circle_data.histogram,
           1, /* dims */
           &circle_data.histogram_size,
           &prange);

  normalize(circle_data.histogram, circle_data.histogram, 0, 255, CV_MINMAX);

#ifdef CONFIG_DEBUG_HIGHGUI
  log_color_hist(reinterpret_cast<size_t>(this) * 100 + 10,
                 circle_data.histogram, circle_data.histogram_size);
#endif

  /* Mark down the radius */
  circle_data.radius = mark.radius;
  return true;
}

/**
 * @param[in]   data      image to preprocess
 * @param[in]   circle_data
 * @param[in]   threshold HSV threshoold
 *
 * @return  grayscale image with circles to search
 */
cv::Mat CircleTracker::PreprocessImage(const cv::Mat &data,
                                       const CircleData &circle_data,
                                       const double threshold) const {
  cv::Mat hsv;

  cvtColor(data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hsv_components[3];
  cv::split(hsv, hsv_components);

  /* Prepare denoise kernel */
  auto radius = parameters().Get(Parameters::TEMPLATE_RADIUS);
  cv::Size blur_size(radius, radius);

  blur_size.width += 1 - (blur_size.width % 2);
  blur_size.height += 1 - (blur_size.height % 2);

  /* Caclulate backprojection */
  const float *prange = circle_data.hrange;
  cv::Mat backproj;
  cv::calcBackProject(&hsv_components[0], 1,
                  0, /* channels */
                  circle_data.histogram,
                  backproj,
                  &prange);

  /* Crop it with to known boundaries only */
  cv::Mat mask;
  cv::inRange(hsv,
        Scalar(circle_data.hrange[0], circle_data.srange[0],
               circle_data.vrange[0]),
        Scalar(circle_data.hrange[1], circle_data.srange[1],
               circle_data.vrange[1]),
        mask);
  backproj &= mask;

  /* Denoise */
  cv::GaussianBlur(backproj, backproj, blur_size, 0);
  return backproj;
}

/**
 * @return Match score [0, 1] of the best circle (0 for none)
 */
double CircleTracker::CirclesToMark(
    const cv::Mat &data,
    const CircleVector &circles,
    Mark *mark) const {

#ifdef CONFIG_DEBUG_HIGHGUI
  auto circ_mat = data.clone();
#endif

  /*
   * If there are multiple cirles detected, use the one whose area has biggest
   * average value of backprojection
   */
  cv::Mat mask(data.size(), data.type());
  double best = -1;
  int best_idx = -1;

  for (int i = 0; i < circles.size(); ++i) {
    /* Prepare mask */
    mask.setTo(cv::Scalar(0, 0, 0));
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    cv::circle(mask, center, radius, cv::Scalar(255, 255, 255));

    auto mean = cv::mean(data, mask);
    if (mean[0] > best) {
      best = mean[0];
      best_idx = i;
    }
#ifdef CONFIG_DEBUG_HIGHGUI
    DEBUG("%s: circle mean: %f", __func__, mean[0]);
    cv::circle(circ_mat, center, radius, cv::Scalar(255, 100, 0), 2);
#endif
  }

  if (best_idx < 0) {
#ifdef CONFIG_DEBUG_HIGHGUI
    log_mat(reinterpret_cast<size_t>(this) * 100 + 3, circ_mat);
#endif
    return 0;
  }

  /* Compose answer */
  auto circle = circles[best_idx];
  mark->type = Mark::kCircle;
  mark->center = Point2(circle[0], circle[1]);
  mark->radius = circle[2];


#ifdef CONFIG_DEBUG_HIGHGUI
  cv::circle(circ_mat, mark->center, mark->radius, cv::Scalar(100, 255, 0), 2);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 3, circ_mat);
#endif

  /* Normalize score */
  return best / 255;
}

} // namespace dove_eye
