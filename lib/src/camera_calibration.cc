#include "dove_eye/camera_calibration.h"

#include <cassert>
#include <vector>

#include "dove_eye/logging.h"

using cv::calibrateCamera;
using cv::Point3f;
using cv::stereoCalibrate;
using std::vector;

namespace dove_eye {

CameraCalibration::CameraCalibration(const CameraIndex camera_count,
                                     const CalibrationPattern &pattern)
    : camera_count_(camera_count),
      pattern_(pattern),
      image_points_(camera_count),
      camera_states_(camera_count, kUnitialized),
      camera_parameters_(camera_count),
      pair_states_(camera_count, kUnitialized),
      pair_parameters_(camera_count),
      pairs_(CameraPair::GenerateArray(camera_count)) {
}

bool CameraCalibration::MeasureFrameset(const Frameset &frameset) {
  assert(frameset.Size() == camera_count_);
  bool result = true;

  /*
   * First we search for pattern in each single camera,
   * when both camera from a pair are calibrated, we estimate pair parameters.
   */
  for (CameraIndex cam = 0; cam < camera_count_; ++cam) {
    if (!frameset.IsValid(cam)) {
      result = result && (camera_states_[cam] == kReady);
      continue;
    }

    Point2Vector image_points;

    switch (camera_states_[cam]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_.Match(frameset[cam].data, &image_points)) {
          image_points_[cam].push_back(image_points);
          camera_states_[cam] = kCollecting;
        }

        if (image_points_[cam].size() >= frames_to_collect_) {
          vector<Point3Vector> object_points(image_points_[cam].size(),
              pattern_.ObjectPoints());

          auto error = calibrateCamera(object_points, image_points_[cam],
              frameset[cam].data.size(),
              camera_parameters_[cam].camera_matrix,
              camera_parameters_[cam].distortion_coefficients,
              cv::noArray(), cv::noArray());
          DEBUG("Camera %i calibrated, reprojection error %f", cam, error);

          camera_states_[cam] = kReady;
          image_points_[cam].clear(); /* Will use it for pair calibration */
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  for (auto pair : pairs_) {
    auto cam1 = pair.cam1;
    auto cam2 = pair.cam2;

    if (camera_states_[cam1] != kReady || camera_states_[cam2] != kReady) {
      result = result && (pair_states_[pair.index] == kReady);
      continue;
    }
    if (!frameset.IsValid(cam1) || !frameset.IsValid(cam2)) {
      result = result && (pair_states_[pair.index] == kReady);
      continue;
    }

    Point2Vector image_points1, image_points2;
    cv::Mat dummy_R, dummy_T;

    switch (pair_states_[pair.index]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_.Match(frameset[cam1].data, &image_points1) &&
            pattern_.Match(frameset[cam2].data, &image_points2)) {
          image_points_[cam1].push_back(image_points1);
          image_points_[cam2].push_back(image_points2);

          pair_states_[pair.index] = kCollecting;
        }
        DEBUG("Matching pair %i, %i", cam1, cam2);

        /* We add frames in lockstep, thus read size from cam1 only. */
        if (image_points_[cam1].size() >= frames_to_collect_) {
          DEBUG("Calibrating pair %i, %i", cam1, cam2);
          vector<Point3Vector> object_points(image_points_[cam1].size(),
              pattern_.ObjectPoints());

          // FIXME Getting size more centrally probably.
          auto error = stereoCalibrate(object_points, image_points_[cam1],
              image_points_[cam2],
              camera_parameters_[cam1].camera_matrix,
              camera_parameters_[cam1].distortion_coefficients,
              camera_parameters_[cam2].camera_matrix,
              camera_parameters_[cam2].distortion_coefficients,
              cv::Size(1, 1), /* not actually used as we already know camera matrix */
              dummy_R,
              dummy_T,
              pair_parameters_[pair.index].essential_matrix, /* E */
              cv::noArray()); /* F */

          DEBUG("Pair %i, %i calibrated, reprojection error %f", cam1, cam2,
                error);

          pair_states_[pair.index] = kReady;
          image_points_[cam1].clear();
          image_points_[cam2].clear();
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  return result;
}

} // namespace dove_eye
