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

  Point2 match_point;
  if (!Search(frame.data, tracker_data, nullptr, &epiline_mask, thr,
              &match_point)) {
    return false;
  }

  /* Store template from current frame for future matching */
  Mark dummy_mark(Mark::kCircle);
  dummy_mark.center = match_point;
  // TODO here'd be tracker_data to radius function
  dummy_mark.radius = parameters().Get(Parameters::TEMPLATE_RADIUS);

  if (!InitTrackerData(frame.data, dummy_mark)) {
    return false;
  }
  initialized(true);

  *result = match_point;
  kalman_filter().Reset(frame.timestamp, *result);

  return true;
}


bool SearchingTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto f = parameters().Get(Parameters::TEMPLATE_SEARCH_FACTOR);
  const auto thr = parameters().Get(Parameters::TEMPLATE_THRESHOLD);

  auto expected = kalman_filter().Predict(frame.timestamp);
  Point2 match_point;

  const auto roi = DataToRoi(tracker_data(), expected, f);
  
  if (!Search(frame.data, tracker_data(), &roi, nullptr, thr, &match_point)) {
    return false;
  }

  *result = match_point;
  kalman_filter().Update(frame.timestamp, *result);
  return true;
}

bool SearchingTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto thr = parameters().Get(Parameters::TEMPLATE_THRESHOLD);

  Point2 match_point;
  if (!Search(frame.data, tracker_data(), nullptr, nullptr, thr, &match_point)) {
    return false;
  }

  *result = match_point;
  kalman_filter_.Update(frame.timestamp, *result);
  return true;
}

} // end namespace dove_eye
