#ifndef DOVE_EYE_CAMERA_CALIBRATION_H_
#define	DOVE_EYE_CAMERA_CALIBRATION_H_

#include <vector>

#include <opencv2/opencv.hpp>

#include <dove_eye/calibration_pattern.h>
#include <dove_eye/camera_pair.h>
#include <dove_eye/frameset.h>
#include <dove_eye/types.h>

namespace dove_eye {

class CameraCalibration {
 public:
  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };
 
  struct CameraParameters {
    cv::Mat cameraMatrix;
    cv::Mat distortionCoefficients;
  };

  struct PairParameters {
    cv::Mat essentialMatrix;
  };
  
  CameraCalibration(const CameraIndex cameraCount,
                    const CalibrationPattern &pattern);
  
  /** Search for pattern in frameset and use it for calibration.
   *
   * \return True when calibration finished successfully, false otherwise.
   */
  bool MeasureFrameset(const Frameset &frameset);

  // FIXME better for indvidual cameras/pairs
  void Reset();

  CameraParameters CameraResult(const CameraIndex index);

  PairParameters PairResult(const CameraIndex index);

  MeasurementState CameraState(const CameraIndex index);
  
  MeasurementState PairState(const CameraIndex index1, const CameraIndex index2);
  
 private:
  const CameraIndex cameraCount_;

  const int framesToCollect_ = 10;

  const CalibrationPattern &pattern_;

  std::vector<std::vector<Point2Vector>> imagePoints_;

  std::vector<MeasurementState> cameraStates_;
  std::vector<CameraParameters> cameraParameters_;

  std::vector<MeasurementState> pairStates_;
  std::vector<PairParameters> pairParameters_;

  CameraPair::PairArray pairs_;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_CAMERA_CALIBRATION_H_ */

