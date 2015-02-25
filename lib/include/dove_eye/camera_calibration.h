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

  CameraParameters CameraResult(const CameraIndex index);

  PairParameters PairResult(const CameraIndex index);

  MeasurementState CameraState(const CameraIndex index);
  
  MeasurementState PairState(const CameraIndex index1, const CameraIndex index2);
  
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

#endif	/* DOVE_EYE_CAMERA_CALIBRATION_H_ */

