#ifndef DOVE_EYE_CAMERA_CALIBRATION_H_
#define DOVE_EYE_CAMERA_CALIBRATION_H_

#include <cassert>
#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_pattern.h"
#include "dove_eye/camera_pair.h"
#include "dove_eye/frameset.h"
#include "dove_eye/types.h"

namespace dove_eye {

class CameraCalibration {
 public:
  struct CameraParameters {
    cv::Mat camera_matrix;
    cv::Mat distortion_coefficients;
  };

  struct PairParameters {
    cv::Mat essential_matrix;
  };

  CameraCalibration(const CameraIndex arity,
                    CalibrationPattern const *pattern);

  /** Search for pattern in frameset and use it for calibration.
   *
   * \return True when calibration finished successfully, false otherwise.
   */
  bool MeasureFrameset(const Frameset &frameset);

  // FIXME better for indvidual cameras/pairs
  void Reset();

  CameraIndex Arity() const {
    return arity_;
  }

  const CameraParameters &camera_result(const CameraIndex cam) const {
    assert(cam < Arity());
    assert(camera_states_[cam] == kReady);

    return camera_parameters_[cam];
  }

  const PairParameters &pair_result(const CameraIndex index) const {
    assert(index < CameraPair::Pairity(Arity()));
    assert(pair_states_[index] == kReady);

    return pair_parameters_[index];
  }

  double CameraProgress(const CameraIndex cam) const;
 
  double PairProgress(const CameraIndex index) const;
 
  const CameraPair::PairArray &pairs() const {
    return pairs_;
  }

 private:
  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };

  const CameraIndex arity_;

  const int frames_to_collect_ = 10;

  std::unique_ptr<const CalibrationPattern> pattern_;

  std::vector<std::vector<Point2Vector>> image_points_;

  std::vector<MeasurementState> camera_states_;
  std::vector<CameraParameters> camera_parameters_;

  std::vector<MeasurementState> pair_states_;
  std::vector<PairParameters> pair_parameters_;

  CameraPair::PairArray pairs_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CAMERA_CALIBRATION_H_

