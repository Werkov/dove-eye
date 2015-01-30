#ifndef DOVE_EYE_TIME_CALIBRATION_H_
#define	DOVE_EYE_TIME_CALIBRATION_H_

#include <opencv2/opencv.hpp>

#include <dove_eye/frame.h>

/**
 * \see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
 */
namespace dove_eye {

class TimeCalibration {
 public:
  typedef Frame::Timestamp ResultType;

  enum MeasurementState {
    kUnitialized,
    kCollecting,
    kReady
  };

  bool MeasureFrame(const Frame &frame);

  void Reset();

  ResultType result();

  inline MeasurementState state() {
    return state_;
  }
 private:
  MeasurementState state_ = kUnitialized;
  const int framesToCollect_ = 10;
  int frames_ = 0;
  ResultType processingSum_ = 0;

  // TODO returned type (consider real clock)
  Frame::Timestamp DetectTime(const cv::Mat &data);
};

} // namespace DoveEye

#endif	/* DOVE_EYE_TIME_CALIBRATION_H_ */

