#include "dove_eye/tracker.h"

#include <cassert>
#include <utility>

#include <opencv2/opencv.hpp>

#include "dove_eye/camera_pair.h"
#include "dove_eye/logging.h"

using cv::computeCorrespondEpilines;
using cv::projectPoints;
using cv::undistortPoints;

namespace dove_eye {

Tracker::Tracker(const CameraIndex arity, const InnerTracker &inner_tracker)
    : arity_(arity),
      positset_(arity_),
      trackstates_(arity_, kUninitialized),
      trackers_(arity_),
      distorted_input_(false),
      calibration_data_(nullptr),
      location_valid_(false) {
  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    trackers_[cam] = std::move(InnerTrackerPtr(inner_tracker.Clone()));
  }
}


/**
 * @return  true when mark is accepted, false otherwise
 */
Positset Tracker::SetMark(const Frameset &frameset, const CameraIndex cam,
                      const InnerTracker::Mark mark, bool project_other) {
  assert(cam < arity_);

  auto tracker = trackers_[cam].get();
  auto success = tracker->InitializeTracking(frameset[cam], mark, &positset_[cam]);
  positset_.SetValid(cam, success);
  DEBUG("%i, %i init", cam, success);
  if (success) {
    trackstates_[cam] = kTracking;
  }

  if (success && project_other) {
    for (CameraIndex o_cam = 0; o_cam < arity_; ++o_cam) {
      if (o_cam == cam) {
        continue;
      }
      auto epiline = CalculateEpiline(positset_[cam], cam, o_cam);
      auto &tracker_data = trackers_[cam]->tracker_data();
      auto o_success = trackers_[o_cam]->InitializeTracking(frameset[o_cam],
                                                            epiline,
                                                            tracker_data,
                                                            &positset_[o_cam]);
      positset_.SetValid(o_cam, o_success);
      DEBUG("%i, %i init", o_cam, o_success);
      if (o_success) {
        trackstates_[o_cam] = kTracking;
      }

      /* All projections must succeed to accept the mark */
      success = success && o_success;
    }
  }

  return positset_;
}

Positset Tracker::Track(const Frameset &frameset) {
  assert(frameset.Arity() == arity_);

  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    (void)TrackSingle(cam, frameset[cam]);
  }

  return positset_;
}

bool Tracker::TrackSingle(const CameraIndex cam, const Frame &frame) {
  auto tracker = trackers_[cam].get();

  //DEBUG("%s(%i) entry state: %i", __func__, cam, trackstates_[cam]);

  switch (trackstates_[cam]) {
    case kUninitialized: {
      /* empty */
      break;
    }

    case kTracking: {
      if (!tracker->Track(frame, &positset_[cam])) {
        trackstates_[cam] = kLost;
        DEBUG("tracker(%i) lost", cam);
        positset_.SetValid(cam, false);
      }
      break;
    }

    case kLost: {
      /* First try re-initialization from knowledge of projection */
      if (location_valid_) {
        auto guess = ReprojectLocation(location_, cam);
        if (tracker->ReinitializeTracking(frame, guess, &positset_[cam])) {
          trackstates_[cam] = kTracking;
          DEBUG("tracker(%i) found from projection", cam);
          positset_.SetValid(cam, true);
          break;
        }
      }

      /*
       * Second fallback is re-initalization on epiline (i.e. not enough data
       * to have full location)
       */
      CameraIndex o_cam = 0;
      bool exists_posit = false;
      for (; o_cam < arity_; ++o_cam) {
        if (o_cam == cam) {
          continue;
        }
        if (positset_.IsValid(o_cam)) {
          exists_posit = true;
          break;
        }
      }
      if (exists_posit) {
        auto epiline = CalculateEpiline(positset_[o_cam], o_cam, cam);
        if (tracker->ReinitializeTracking(frame, epiline, &positset_[cam])) {
          trackstates_[cam] = kTracking;
          DEBUG("tracker(%i) found from epiline of %i", cam, o_cam);
          positset_.SetValid(cam, true);
          break;
        }
      }

      /*
       * Lastly try global search on the frame
       */
      if (tracker->ReinitializeTracking(frame, &positset_[cam])) {
        trackstates_[cam] = kTracking;
        DEBUG("tracker(%i) found from global search", cam);
        positset_.SetValid(cam, true);
        break;
      }

      assert(positset_.IsValid(cam) == false);
      break;
    }
  }

  if (positset_.IsValid(cam) && distorted_input()) {
    positset_[cam] = Undistort(positset_[cam], cam);
  }

  //DEBUG("%s(%i) exit state: %i, return: %i", __func__, cam, trackstates_[cam],
   //     positset_.IsValid(cam));

  return positset_.IsValid(cam);
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

InnerTracker::Epiline Tracker::CalculateEpiline(
      const Posit posit,
      const CameraIndex marked_cam,
      const CameraIndex cam) const {
  assert(calibration_data_);

  Point2Vector points({static_cast<Point2>(posit)});
  Point3Vector lines;

  CameraIndex index = CameraPair::Index(arity_, marked_cam, cam);
  CameraIndex image = (marked_cam < cam) ? 1 : 2;
  auto &F = calibration_data_->pair_parameters(index).fundamental_matrix;

  computeCorrespondEpilines(points, image, F, lines);

  return lines.front();
}


Point2 Tracker::ReprojectLocation(const Location location,
                                       const CameraIndex cam) const {
  assert(calibration_data_);

  Point3Vector object_points({static_cast<Point3>(location)});
  Point2Vector image_points;

  auto R = calibration_data_->CameraRotation(cam);
  auto t = calibration_data_->CameraTranslation(cam);
  auto &C = calibration_data_->camera_parameters(cam).camera_matrix;
  auto &D = calibration_data_->camera_parameters(cam).distortion_coefficients;


  if (distorted_input()) {
    projectPoints(object_points, R, t, C, D, image_points);
  } else {
    projectPoints(object_points, R, t, C, cv::noArray(), image_points);
  }

  return image_points.front();
}

} // namespace dove_eye
