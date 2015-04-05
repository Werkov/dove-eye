#include "dove_eye/template_tracker.h"

#include <opencv2/opencv.hpp>

using cv::Mat;
using cv::line;
using cv::matchTemplate;
using cv::minMaxLoc;
using cv::Rect;
using dove_eye::Parameters;

namespace dove_eye {

bool TemplateTracker::InitializeTracking(const Frame &frame, Posit *result) {
  assert(mark_set_);

  const auto radius = parameters_.Get(Parameters::TEMPLATE_RADIUS);
  if (!TakeTemplate(frame.data, static_cast<Point2>(mark_), radius)) {
    return false;
  }

  initialized_ = true;

  *result = mark_;
  previous_match_ = *result;

  return true;
}

bool TemplateTracker::InitializeTracking(
      const Frame &frame,
      const Epiline epiline,
      const TrackerData *tracker_data,
      Posit *result) {

  const auto template_data = static_cast<const TemplateData *>(tracker_data);
  const auto epiline_mask = EpilineToMask(frame.data, epiline);
  // FIXME Use different threshold for foreign match?
  const auto thr = parameters_.Get(Parameters::TEMPLATE_THRESHOLD);

  Point2 match_point;
  if (!Match(frame.data, *template_data, nullptr, &epiline_mask, thr,
             &match_point)) {
    return false;
  }

  /* Store template from current frame for future matching */
  const auto radius = parameters_.Get(Parameters::TEMPLATE_RADIUS);
  if (!TakeTemplate(frame.data, match_point, radius)) {
    return false;
  }
  initialized_ = true;

  /* Actual position is in the middle of template */
  *result = match_point;
  previous_match_ = *result;

  return true;
}

bool TemplateTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized_);

  const auto f = parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR);
  const auto thr = parameters_.Get(Parameters::TEMPLATE_THRESHOLD);

  Rect roi(previous_match_.x - f * data_.radius,
           previous_match_.y - f * data_.radius,
           2 * f * data_.radius, 2 * f * data_.radius);

  Point2 match_point;
  if (!Match(frame.data, data_, &roi, nullptr, thr, &match_point)) {
    return false;
  }

  *result = match_point;
  previous_match_ = *result;
  return true;
}

bool TemplateTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
  assert(initialized_);

  const auto thr = parameters_.Get(Parameters::TEMPLATE_THRESHOLD);

  Point2 match_point;
  if (!Match(frame.data, data_, nullptr, nullptr, thr, &match_point)) {
    return false;
  }

  *result = match_point;
  previous_match_ = *result;
  return true;
}

InnerTracker *TemplateTracker::Clone() const {
  assert(!initialized_);

  return new TemplateTracker(*this);
}

cv::Mat TemplateTracker::EpilineToMask(const cv::Mat &data,
                                       const Epiline epiline) const {
  /*
   * Epiline:
   *    a*x + b*y + c = 0
   *
   *    y = (a*x + c) / -b
   *    x = (b*y + c) / -a
   */

  Point2 p1;
  Point2 p2;
  if (abs(epiline[1]) > 1e-1) {
    p1.x = 0;
    p1.y = (epiline[0] * p1.x + epiline[3]) / -epiline[1];

    p2.x = data.cols;
    p2.y = (epiline[0] * p2.x + epiline[3]) / -epiline[1];
  } else {
    p1.y = 0;
    p1.x = (epiline[1] * p1.y + epiline[3]) / -epiline[0];

    p2.y = 0;
    p2.x = (epiline[1] * p2.y + epiline[3]) / -epiline[0];
  }
  
  cv::Mat mask(data.size(), CV_8U);
  mask.setTo(cv::Scalar(0, 0, 0));

  // FIXME Possibly use diffent parameters to specify epiline mask
  auto thickness = parameters_.Get(Parameters::TEMPLATE_RADIUS) *
      parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR) / 2;

  line(mask, p1, p2, cv::Scalar(255, 255, 255), thickness);

  return mask;
}

bool TemplateTracker::TakeTemplate(const cv::Mat &data, const Point2 point,
                                   const double radius) {
  if (point.x < radius || point.x >= data.cols - radius ||
      point.y < radius || point.y >= data.rows - radius) {
    return false;
  }

  Rect roi(point.x - radius, point.y - radius, 2 * radius, 2 * radius);

  /* We don't want to have the template overwritten */
  data_.search_template = data(roi).clone();
  data_.radius = radius;

  return true;
}

bool TemplateTracker::Match(
      const cv::Mat &data,
      const TemplateData &tpl,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Point2 *result,
      double *quality) const {
  Rect cropped_roi = Rect(cv::Point(0, 0), data.size());
  if (roi) {
    cropped_roi &= *roi;
  }

  // TODO is this preallocatin necessary?
  Mat match_result(cropped_roi.height - tpl.search_template.rows + 1,
                  cropped_roi.width - tpl.search_template.cols + 1, CV_32FC1);
  matchTemplate(data(cropped_roi), tpl.search_template, match_result,
                    CV_TM_SQDIFF_NORMED);

  double min_diff;
  cv::Point min_loc;

  if (mask) {
    Rect mask_roi(cv::Point(0, 0), match_result.size());
    cv::Mat cropped_mask((*mask)(mask_roi));

    assert(match_result.rows = cropped_mask.rows);
    assert(match_result.cols = cropped_mask.cols);

    minMaxLoc(match_result, &min_diff, nullptr, &min_loc, nullptr, cropped_mask);
  } else {
    minMaxLoc(match_result, &min_diff, nullptr, &min_loc, nullptr);
  }

  if (min_diff >= threshold) {
    return false;
  }
  // TODO return false also when minumum is shallow (i.e. not unique match)

  /* Actual match is in the middle of the template */
  *result = Point2(min_loc.x, min_loc.y) + Point2(tpl.radius, tpl.radius);
  /* ...and has offset of the ROI */
  *result = *result + Point2(cropped_roi.x, cropped_roi.y);

  if (quality) {
    *quality = min_diff;
  }
  
  return true;
}


} // namespace dove_eye
