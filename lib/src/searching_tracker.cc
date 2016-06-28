#include "dove_eye/searching_tracker.h"


#include "dove_eye/cv_logging.h"
#include "dove_eye/logging.h"

namespace dove_eye {

bool SearchingTracker::InitializeTracking(const Frame &frame, const Mark mark,
                                          Posit *result) {
  if (!InitTrackerData(frame.data, mark)) {
    return false;
  }

  initialized(true);

  InitializeKalmanFilter();
  const auto posit = MarkToPosit(mark);
  *result = kalman_filter().Reset(frame.timestamp, posit);

  return true;
}

bool SearchingTracker::InitializeTracking(
    const Frame &frame,
    const Epiline epiline,
    const TrackerData &tracker_data,
    Posit *result) {

  // FIXME Possibly use diffent parameters to specify epiline mask
  auto thickness = parameters().Get(Parameters::TEMPLATE_RADIUS) *
      parameters().Get(Parameters::SEARCH_FACTOR);

  const auto epiline_mask = EpilineToMask(frame.data.size(), thickness, epiline);

  // FIXME Use different threshold for foreign search data?
  const auto thr = parameters().Get(Parameters::SEARCH_THRESHOLD);

  Mark match_mark;
  // TODO Remove non-const cast! Do it properly when projection is working
  if (!Search(frame.data, (TrackerData &)tracker_data, nullptr, &epiline_mask, thr,
              &match_mark)) {
    return false;
  }

  /* Store template from current frame for future matching */
  if (!InitTrackerData(frame.data, match_mark)) {
    return false;
  }
  initialized(true);

  InitializeKalmanFilter();
  const auto posit = MarkToPosit(match_mark);
  *result = kalman_filter().Reset(frame.timestamp, posit);

  return true;
}


bool SearchingTracker::Track(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto f = parameters().Get(Parameters::SEARCH_FACTOR);
  const auto thr = parameters().Get(Parameters::SEARCH_THRESHOLD);
  const auto min_speed = parameters().Get(Parameters::SEARCH_MIN_SPEED);

  /* Calculate expected position */
  auto expected = kalman_filter().Predict(frame.timestamp);
  auto velocity = kalman_filter().PredictChange(frame.timestamp);
  const auto roi = DataToRoi(tracker_data(), expected, f);
  bool moving = (cv::norm(velocity) > min_speed);
  // TODO temporarily disable motion detection
  moving = false;

  DEBUG("%p->%s, expected: [%f, %f], velocity [%f, %f], moving: %i",
        this, __func__,
        expected.x, expected.y,
        velocity.x, velocity.y,
        moving);

  /* Filter movement */
  cv::Mat fg_mask;
  /* 1: learn, 0: not learn */
  bg_subtractor()(frame.data.clone(), fg_mask);

  log_mat(reinterpret_cast<size_t>(this) * 1000 + 42, fg_mask);

  auto fg_mask_ptr = moving ? &fg_mask : nullptr;

  /* Search for object */
  Mark match_mark;
  if (!Search(frame.data, tracker_data(), &roi, fg_mask_ptr, thr, &match_mark)) {
    return false;
  }

  /* Use result */
  const auto posit = MarkToPosit(match_mark);
  *result = kalman_filter().Update(frame.timestamp, posit);
  return true;
}

bool SearchingTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
  assert(initialized());

  const auto thr = parameters().Get(Parameters::SEARCH_THRESHOLD);

  // FIXME Would be expectation be of any use here?

  /* Assume object is moving, i.e. applying foreground mask */
  cv::Mat fg_mask;
  bg_subtractor()(frame.data.clone(), fg_mask, -1);

  Mark match_mark;
  if (!Search(frame.data, tracker_data(), nullptr, &fg_mask, thr, &match_mark)) {
    /* Fallback without mask */
    if (!Search(frame.data, tracker_data(), nullptr, nullptr, thr, &match_mark)) {
      return false;
    }
  }

  auto posit = MarkToPosit(match_mark);
  *result = kalman_filter().Update(frame.timestamp, posit);
  return true;
}

void SearchingTracker::InitializeKalmanFilter() {
  const auto process_var = parameters().Get(Parameters::SEARCH_KF_PROC_V);
  const auto observation_var = parameters().Get(Parameters::SEARCH_KF_OBS_V);

  kalman_filter().Init(process_var, observation_var);
}
} // end namespace dove_eye
