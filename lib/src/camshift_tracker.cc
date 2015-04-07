#include "dove_eye/camshift_tracker.h"

#include <opencv2/opencv.hpp>

#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

#define LOG_SEARCH_MAT

using cv::calcBackProject;
using cv::calcHist;
using cv::cvtColor;
using cv::mixChannels;
using cv::normalize;

namespace dove_eye {


bool CamshiftTracker::InitTrackerData(const cv::Mat &data, const Mark &mark) {
  assert(mark.type == Mark::kRectangle);


  cv::Rect roi(mark.top_left, mark.top_left + mark.size);

  if (roi.br().x >= data.cols ||
      roi.br().y >= data.rows) {
    return false;
  }

  auto roi_data = data(roi);
  auto preprocessed = PreprocessImage(roi_data);
  const float *prange = data_.hrange;

  /* Calculate histogram */
  calcHist(&preprocessed,
           1, /* no. of images */
           nullptr, /* channels, can be nullptr when dims == no. of images */
           cv::Mat(), /* mask */
           data_.histogram,
           1, /* dims */
           &data_.histogram_size,
           &prange);
           //const_cast<const float **>(&data_.hrange)); /* ranges */

#ifdef LOG_SEARCH_MAT
  log_color_hist(reinterpret_cast<size_t>(this) * 100 + 10,
                 data_.histogram, data_.histogram_size);
#endif

  normalize(data_.histogram, data_.histogram, 0, 255, CV_MINMAX);


  data_.size = roi.size();
  data_.track_window = roi;

#ifdef LOG_SEARCH_MAT
  log_color_hist(reinterpret_cast<size_t>(this) * 100 + 11,
                 data_.histogram, data_.histogram_size);
#endif

  return true;
}

/** Wrapper for OpenCV function CamShift
 * @see SearchingTracker::Search()
 */
bool CamshiftTracker::Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result,
      double *quality) const {

  const CamshiftData &hist_data = static_cast<const CamshiftData &>(tracker_data);
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
  auto preprocessed = PreprocessImage(data_roi);
  const float *prange = hist_data.hrange;

  cv::Mat backproj;
  calcBackProject(&preprocessed, 1,
                  0, /* channels */
                  hist_data.histogram,
                  backproj,
                  &prange);

  if (mask) {
    backproj &= *mask;
  }

  auto track_window = hist_data.track_window;
  /*auto track_box = CamShift(backproj, track_window,
      cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));*/

  auto steps = meanShift(backproj, track_window,
      cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));

  auto &rect = track_window;
#ifdef LOG_SEARCH_MAT
  log_mat(reinterpret_cast<size_t>(this) * 100 + 1, preprocessed);
  log_mat(reinterpret_cast<size_t>(this) * 100 + 2, data_roi);

  /*cv::Point2f vertices[4];
  track_box.points(vertices);
  for (int i = 0; i < 4; i++) {
    cv::line(backproj, vertices[i], vertices[(i+1)%4], cv::Scalar(255,255,255));
  }*/
  cv::rectangle(backproj, rect, cv::Scalar(255, 255, 255));

  log_mat(reinterpret_cast<size_t>(this) * 100 + 3, backproj);
#endif

  /*if (track_box.size.area() == 0) { // or threshold?
    return false;
  }*/

  //auto &rect = track_box.boundingRect();
  data_.track_window = rect;

  result->type = Mark::kRectangle;
  result->top_left = rect.tl() + extended_roi.tl();
  result->size = Point2(rect.size());

  if (quality) {
    // FIXME definition of quality
    *quality = 1;
  }
  
  return true;
}

cv::Mat CamshiftTracker::PreprocessImage(const cv::Mat &data) const {
  cv::Mat hsv;

  cvtColor(data, hsv, cv::COLOR_BGR2HSV);
  cv::Mat hue(hsv.size(), hsv.depth());

  int ch[] = {0, 0};
  mixChannels(&hsv, 1, &hue, 1, ch, 1);

  return hue;
}

} // namespace dove_eye
