#ifndef DOVE_EYE_CAMERA_CALIBRATION_H_
#define	DOVE_EYE_CAMERA_CALIBRATION_H_

#include <opencv2/opencv.hpp>

#include <dove_eye/frameset.h>

namespace dove_eye {

class CameraCalibration {
 public:
  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };
  
  CameraCalibration(const CameraIndex cameraCount);
  
  bool MeasureFrameset(const Frameset &frame);

  void Reset();

  // TODO design type for result

  MeasurementState CameraState(const CameraIndex index);
  
  MeasurementState PairState(const CameraIndex index1, const CameraIndex index2);
  
 private:
  const CameraIndex cameraCount_;
};

} // namespace DoveEye

#endif	/* DOVE_EYE_CAMERA_CALIBRATION_H_ */

