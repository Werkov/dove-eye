#ifndef DOVE_EYE_CAMERA_CALIBRATION_H_
#define DOVE_EYE_CAMERA_CALIBRATION_H_

#include <cassert>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_pattern.h"
#include "dove_eye/camera_pair.h"
#include "dove_eye/frameset.h"
#include "dove_eye/types.h"

namespace dove_eye {

class CameraCalibration {
 public:
  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };

  struct CameraParameters {
    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;
  };

  struct PairParameters {
    cv::Mat essential_matrix;
  };

  CameraCalibration(const CameraIndex camera_count,
                    const CalibrationPattern &pattern);

  /** Search for pattern in frameset and use it for calibration.
   *
   * \return True when calibration finished successfully, false otherwise.
   */
  bool MeasureFrameset(const Frameset &frameset);

  // FIXME better for indvidual cameras/pairs
  void Reset();

  CameraIndex camera_count() const {
    return camera_count_;
  }

  const CameraParameters &camera_result(const CameraIndex cam) const {
    assert(cam < camera_count());
    assert(camera_state(cam) == kReady);

    return camera_parameters_[cam];
  }

  const PairParameters &pair_result(const CameraIndex cam1,
                                    const CameraIndex cam2) const {
    assert(cam1 < camera_count());
    assert(cam2 < camera_count());
    assert(pair_state(cam1, cam2) == kReady);

    return pair_parameters_[CameraPair::Index(camera_count(), cam1, cam2)];
  }

  MeasurementState camera_state(const CameraIndex cam) const {
    assert(cam < camera_count());

    return camera_states_[cam];
  }

  MeasurementState pair_state(const CameraIndex cam1,
                              const CameraIndex cam2) const {
    assert(cam1 < camera_count());
    assert(cam2 < camera_count());

    return pair_states_[CameraPair::Index(camera_count(), cam1, cam2)];
  }

  const CameraPair::PairArray &pairs() const {
    return pairs_;
  }

 private:
  const CameraIndex camera_count_;

  const int frames_to_collect_ = 10;

  const CalibrationPattern &pattern_;

  std::vector<std::vector<Point2Vector>> image_points_;

  std::vector<MeasurementState> camera_states_;
  std::vector<CameraParameters> camera_parameters_;

  std::vector<MeasurementState> pair_states_;
  std::vector<PairParameters> pair_parameters_;

  CameraPair::PairArray pairs_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CAMERA_CALIBRATION_H_

