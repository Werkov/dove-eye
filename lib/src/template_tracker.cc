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
  assert(mark.x > r);
  assert(mark.y > r);

  Rect roi(mark.x - r, mark.y - r, 2 * r, 2 * r);

  template_ = frame.data(roi);
  radius_ = r;
  previous_match_ = mark;

  *result = mark;
  return true;
}

bool TemplateTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized_);
  const auto f = parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR);

  Rect roi(previous_match_.x - f * radius_, previous_match_.y - f * radius_,
           2 * f * radius_, 2 * f * radius_);
  roi &= Rect(Point(0, 0), frame.data.size());

  Mat matchResult;
  cv::matchTemplate(frame.data(roi), template_, matchResult, CV_TM_SQDIFF);

  // TODO check thresholds and return coordinates
  return false;
}

InnerTracker *TemplateTracker::Clone() const {
  assert(!initialized_);

  return new TemplateTracker(*this);
}

} // namespace dove_eye
