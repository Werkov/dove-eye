#ifndef DOVE_EYE_CAMERA_CALIBRATION_H_
#define DOVE_EYE_CAMERA_CALIBRATION_H_

#include <cassert>
#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_data.h"
#include "dove_eye/calibration_pattern.h"
#include "dove_eye/camera_pair.h"
#include "dove_eye/frameset.h"
#include "dove_eye/parameters.h"
#include "dove_eye/types.h"

namespace dove_eye {

class CameraCalibration {
 public:
  CameraCalibration(const Parameters &parameters,
                    const CameraIndex arity,
                    CalibrationPattern const *pattern);

  /** Search for pattern in frameset and use it for calibration.
   *
   * \return True when calibration finished successfully, false otherwise.
   */
  bool MeasureFrameset(const Frameset &frameset);

  // FIXME better for indvidual cameras/pairs
  void Reset();

  inline CameraIndex Arity() const {
    return arity_;
  }

  double CameraProgress(const CameraIndex cam) const;
 
  double PairProgress(const CameraIndex index) const;

  inline const CalibrationData &Data() const {
    for (auto state : camera_states_) assert(state == kReady);
    for (auto state : pair_states_) assert(state == kReady);

    return data_;
  }
 
  // TODO remove this cache...
  const CameraPair::PairArray &pairs() const {
    return pairs_;
  }

 private:
  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };

  const Parameters &parameters_;

  const CameraIndex arity_;

  int frames_to_collect_;
  int frames_skip_;
  int frame_no_;

  std::unique_ptr<const CalibrationPattern> pattern_;

  std::vector<std::vector<Point2Vector>> image_points_;

  std::vector<MeasurementState> camera_states_;
  std::vector<MeasurementState> pair_states_;

  CalibrationData data_;

  CameraPair::PairArray pairs_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CAMERA_CALIBRATION_H_

