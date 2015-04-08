#include "dove_eye/searching_tracker.h"


#include "dove_eye/logging.h"

namespace dove_eye {

bool SearchingTracker::InitializeTracking(const Frame &frame, Posit *result) {
  assert(mark_set());
  
  if (!InitTrackerData(frame.data, mark())) {
    return false;
  }

  initialized(true);

  *result = MarkToPosit(mark());
  kalman_filter().Reset(frame.timestamp, *result);

  return true;
}

bool SearchingTracker::InitializeTracking(
    const Frame &frame,
    const Epiline epiline,
    const TrackerData &tracker_data,
    Posit *result) {
  
  // FIXME Possibly use diffent parameters to specify epiline mask
  auto thickness = parameters().Get(Parameters::TEMPLATE_RADIUS) *
      parameters().Get(Parameters::TEMPLATE_SEARCH_FACTOR);

  const auto epiline_mask = EpilineToMask(frame.data.size(), thickness, epiline);

  // FIXME Use different threshold for foreign search data?
  const auto thr = parameters().Get(Parameters::TEMPLATE_THRESHOLD);

  Mark match_mark;
  if (!Search(frame.data, tracker_data, nullptr, &epiline_mask, thr,
              &match_mark)) {
    return false;
  }

  /* Store template from current frame for future matching */

  if (!InitTrackerData(frame.data, match_mark)) {
    return false;
  }
  initialized(true);

  *result = MarkToPosit(match_mark);
  kalman_filter().Reset(frame.timestamp, *result);

  return true;
}


bool SearchingTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto f = parameters().Get(Parameters::TEMPLATE_SEARCH_FACTOR);
  const auto thr = parameters().Get(Parameters::TEMPLATE_THRESHOLD);

  auto expected = kalman_filter().Predict(frame.timestamp);
  const auto roi = DataToRoi(tracker_data(), expected, f);

  cv::Mat fg_mask;
  DEBUG("%s", __func__);
  bg_subtractor()(frame.data.clone(), fg_mask, -1);

  Mark match_mark;
  
  if (!Search(frame.data, tracker_data(), &roi, &fg_mask, thr, &match_mark)) {
    return false;
  }

  *result = MarkToPosit(match_mark);
  kalman_filter().Update(frame.timestamp, *result);
  return true;
}

bool SearchingTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto thr = parameters().Get(Parameters::TEMPLATE_THRESHOLD);

  cv::Mat fg_mask;
  DEBUG("%s", __func__);
  bg_subtractor()(frame.data.clone(), fg_mask, -1);

  Mark match_mark;

  if (!Search(frame.data, tracker_data(), nullptr, &fg_mask, thr, &match_mark)) {
    /* Fallback without mask */
    if (!Search(frame.data, tracker_data(), nullptr, nullptr, thr, &match_mark)) {
      return false;
    }
  }

  *result = MarkToPosit(match_mark);
  kalman_filter().Update(frame.timestamp, *result);
  return true;
}

} // end namespace dove_eye