#include "dove_eye/template_tracker.h"

#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::Point;
using cv::Rect;
using dove_eye::Parameters;

namespace dove_eye {

bool TemplateTracker::InitializeTracking(const Frame &frame, const Mark mark,
                                         Posit *result) {
  const auto r = parameters_.Get(Parameters::TEMPLATE_RADIUS);
  if (mark.x < r || mark.x >= frame.data.cols - r ||
      mark.y < r || mark.y >= frame.data.rows - r) {
    return false;
  }

  Rect roi(mark.x - r, mark.y - r, 2 * r, 2 * r);

  initialized_ = true;
  /* We don't want to have the template overwritten */
  template_ = frame.data(roi).clone();
  radius_ = r;
  previous_match_ = mark;

  *result = mark;
  return true;
}

bool TemplateTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized_);

  const auto f = parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR);
  const auto thr = parameters_.Get(Parameters::TEMPLATE_THRESHOLD);

  Rect roi(previous_match_.x - f * radius_, previous_match_.y - f * radius_,
           2 * f * radius_, 2 * f * radius_);
  roi &= Rect(Point(0, 0), frame.data.size());

  Mat match_result(roi.height - template_.rows + 1,
                  roi.width - template_.cols + 1, CV_32FC1);
  cv::matchTemplate(frame.data(roi), template_, match_result,
                    CV_TM_SQDIFF_NORMED);

  double min_diff;
  Point min_loc;

  cv::minMaxLoc(match_result, &min_diff, nullptr, &min_loc, nullptr);
  if (min_diff >= thr) {
    return false;
  }

  /* Actual position is in the middle of template and has offset of ROI */
  *result = min_loc + Point(radius_, radius_) + roi.tl();
  previous_match_ = *result;
  return true;
}

InnerTracker *TemplateTracker::Clone() const {
  assert(!initialized_);

  return new TemplateTracker(*this);
}

} // namespace dove_eye
