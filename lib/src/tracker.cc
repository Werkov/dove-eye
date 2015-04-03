#include "dove_eye/tracker.h"

#include <utility>

#include <opencv2/opencv.hpp>

using cv::undistortPoints;

namespace dove_eye {

Tracker::Tracker(const CameraIndex arity, const InnerTracker &inner_tracker)
    : arity(arity),
      positset_(arity),
      trackstates_(arity, kUninitialized),
      trackers_(arity),
      distorted_input_(false),
      calibration_data_(nullptr) {
  for (CameraIndex cam = 0; cam < arity; ++cam) {
    trackers_[cam] = std::move(InnerTrackerPtr(inner_tracker.Clone()));
  }
}


void Tracker::SetMark(const CameraIndex cam, const InnerTracker::Mark mark,
                      bool project_other) {
  assert(cam < arity);
  // TODO implement projection
  assert(project_other == false);

  positset_[cam] = mark;
  positset_.SetValid(cam, false);

  trackstates_[cam] = kMarkSet;
}

Positset Tracker::Track(const Frameset &frameset) {
  assert(frameset.Arity() == arity);

  for (CameraIndex cam = 0; cam < arity; ++cam) {
    TrackSingle(cam, frameset[cam]);
  }

  return positset_;
}

void Tracker::TrackSingle(const CameraIndex cam, const Frame &frame) {
  switch (trackstates_[cam]) {
    case kUninitialized:
      return;

    case kMarkSet:
      if (trackers_[cam]->InitializeTracking(frame, positset_[cam],
                                            &positset_[cam])) {
        trackstates_[cam] = kTracking;
        positset_.SetValid(cam, true);
      } else {
        /*
         * If initialization failed, do not change state
         * and try it next time.
         */
        positset_.SetValid(cam, false);
      }
      break;

    case kTracking:
      if (!trackers_[cam]->Track(frame, &positset_[cam])) {
        trackstates_[cam] = kLost;
        positset_.SetValid(cam, false);
      }
      break;
    case kLost:
      // TODO re-initialize tracking
      //    e.g. with knowledge from other cameras
      //         or projection...
      break;
  }

  if (positset_.IsValid(cam) && distorted_input()) {
    positset_[cam] = Undistort(positset_[cam], cam);
  }

}

Point2 Tracker::Undistort(const Point2 &point, const CameraIndex cam) const {
  assert(calibration_data_);
  // TODO verify this routine

  cv::Mat result;
  undistortPoints(cv::Mat(point), result,
                  calibration_data_->camera_parameters(cam).camera_matrix,
                  calibration_data_->camera_parameters(cam).distortion_coefficients);

  return Point2(result);
}

} // namespace dove_eye
