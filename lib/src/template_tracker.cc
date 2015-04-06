#include "dove_eye/template_tracker.h"

#include <opencv2/opencv.hpp>

#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

using cv::line;
using cv::matchTemplate;
using cv::meanStdDev;
using cv::minMaxLoc;
using cv::setIdentity;
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
  kalman_filter_.Reset(frame.timestamp, *result);

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

  *result = match_point;
  kalman_filter_.Reset(frame.timestamp, *result);

  return true;
}

bool TemplateTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized_);

  const auto f = parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR);
  const auto thr = parameters_.Get(Parameters::TEMPLATE_THRESHOLD);

  auto exp = kalman_filter_.Predict(frame.timestamp);
  Point2 match_point;

  cv::Rect roi(exp.x - f * data_.radius, exp.y - f * data_.radius,
           2 * f * data_.radius, 2 * f * data_.radius);

  double quality;
  if (!Match(frame.data, data_, &roi, nullptr, thr, &match_point, &quality)) {
    return false;
  }

  *result = match_point;
  kalman_filter_.Update(frame.timestamp, *result);
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
  kalman_filter_.Update(frame.timestamp, *result);
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
    p1.y = (epiline[0] * p1.x + epiline[2]) / -epiline[1];

    p2.x = data.cols;
    p2.y = (epiline[0] * p2.x + epiline[2]) / -epiline[1];
  } else {
    p1.y = 0;
    p1.x = (epiline[1] * p1.y + epiline[2]) / -epiline[0];

    p2.y = data.rows;
    p2.x = (epiline[1] * p2.y + epiline[2]) / -epiline[0];
  }
  
  cv::Mat mask(data.size(), CV_8U);
  mask.setTo(cv::Scalar(0, 0, 0));

  // FIXME Possibly use diffent parameters to specify epiline mask
  auto thickness = parameters_.Get(Parameters::TEMPLATE_RADIUS) *
      parameters_.Get(Parameters::TEMPLATE_SEARCH_FACTOR);

  line(mask, p1, p2, cv::Scalar(255, 255, 255), thickness);

  return mask;
}

bool TemplateTracker::TakeTemplate(const cv::Mat &data, const Point2 point,
                                   const double radius) {
  if (point.x < radius || point.x >= data.cols - radius ||
      point.y < radius || point.y >= data.rows - radius) {
    return false;
  }

  cv::Rect roi(point.x - radius, point.y - radius, 2 * radius, 2 * radius);

  /* We don't want to have the template overwritten */
  data_.search_template = data(roi).clone();
  data_.radius = radius;

  return true;
}

/** Wrapper for OpenCV function matchTemplate
 *
 * @param[in]   data      image to search for template
 * @param[in]   tpl       template data (data and parameters)
 * @param[in]   roi       (optional) region of interest that should be searched
 *                            (in the image)
 * @param[in]   mask      (optional) boolean mask restricting search (in the
 *                            image too)
 * @param[in]   threshold value [0,1] to accept the match (the higher, the
 *                            better)
 * @param[out]  result    image point of the best match
 * @param[out]  quality   (optional) value in [0,1], the higher the better
 *
 * @return      true if sufficient match was found, false otherwise
 */
bool TemplateTracker::Match(
      const cv::Mat &data,
      const TemplateData &tpl,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Point2 *result,
      double *quality) const {

  auto extended_roi = cv::Rect(cv::Point(0, 0), data.size());
  if (roi) {
    extended_roi = cv::Rect(tpl.TopLeft(roi->tl()), tpl.BottomRight(roi->br()));
    extended_roi &= cv::Rect(cv::Point(0, 0), data.size());
  }

  if (extended_roi.width < tpl.search_template.cols ||
      extended_roi.height < tpl.search_template.rows) {
    return false;
  }

  //const int method = CV_TM_SQDIFF_NORMED;
  //const int method = CV_TM_CCORR_NORMED
  const int method = CV_TM_CCOEFF_NORMED;

  cv::Mat match_result;
  matchTemplate(data(extended_roi), tpl.search_template, match_result, method);


  double min_val;
  cv::Point min_loc;
  double max_val;
  cv::Point max_loc;
  cv::Scalar std_dev;

  if (mask) {
    /* Mask is first cropped with same ROI as image */
    auto cropped_mask = (*mask)(extended_roi);

    /* Consequently mask is shifted, which is effectively cropping top-left
     * corner */
    cv::Point mask_br(cropped_mask.cols, cropped_mask.rows);
    cv::Rect shift_rect(-tpl.TopLeft(), mask_br - tpl.BottomRight());

    /* We've worked with (zero-based) coordinates, add one to size */
    shift_rect.width += 1;
    shift_rect.height += 1;

    auto shifted_mask = cropped_mask(shift_rect);

    assert(match_result.rows == shifted_mask.rows);
    assert(match_result.cols == shifted_mask.cols);

    minMaxLoc(match_result, &min_val, &max_val, &min_loc, &max_loc, shifted_mask);
    meanStdDev(match_result, cv::noArray(), std_dev, shifted_mask);
  } else {
    minMaxLoc(match_result, &min_val, &max_val, &min_loc, &max_loc);
    meanStdDev(match_result, cv::noArray(), std_dev);
  }

  const double value = (method == CV_TM_SQDIFF_NORMED) ? (1-min_val) :
      (method == CV_TM_CCORR_NORMED) ? max_val :
      (method == CV_TM_CCOEFF_NORMED) ? (max_val - min_val) : 0;

#if 0
  if (show_mat) {
    cv::Mat masked;
    match_result.copyTo(masked, shifted_mask);

    log_mat((reinterpret_cast<size_t>(this) * 100) + 5, match_result);
    log_mat((reinterpret_cast<size_t>(this) * 100) + 6, masked);
    cv::Mat to_show1 = (match_result - min_val) / value;
    cv::Mat to_show2 = (masked - min_val) / value;
    log_mat((reinterpret_cast<size_t>(this) * 100) + 7, to_show1);
    log_mat((reinterpret_cast<size_t>(this) * 100) + 8, to_show2);
  }
#endif

  DEBUG("quality: %f\t%f\t%f\t%f", value, min_val, max_val, std_dev[0]);

  if (value <= threshold) {
    log_mat(reinterpret_cast<size_t>(this) * 100 + 1, data(extended_roi));
    log_mat(reinterpret_cast<size_t>(this) * 100 + 2, tpl.search_template);

    return false;
  }

  // TODO return false also when minumum is shallow (i.e. not unique match)

  /* Actual match is in the middle of the template */
  const auto loc = (method == CV_TM_SQDIFF_NORMED) ? min_loc :
      (method == CV_TM_CCORR_NORMED) ? max_loc :
      (method == CV_TM_CCOEFF_NORMED) ? (max_loc) : cv::Point();

  cv::Point tpl_offset = -tpl.TopLeft(cv::Point(0, 0));
  *result = Point2(loc.x, loc.y) + Point2(tpl_offset.x, tpl_offset.y);
  /* ...and has offset of the ROI */
  *result = *result + Point2(extended_roi.x, extended_roi.y);


  if (quality) {
    // FIXME definition of quality
    *quality = value;
  }
  
  return true;
}


} // namespace dove_eye
