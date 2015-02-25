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

  ResultType Result();

  inline MeasurementState state() {
    return state_;
  }
 private:
  MeasurementState state_ = kUnitialized;
  const int frames_to_collect_ = 10;
  int frames_ = 0;
  ResultType processing_sum_ = 0;

  // TODO returned type (consider real clock)
  Frame::Timestamp DetectTime(const cv::Mat &data);
};

} // namespace dove_eye

#endif	/* DOVE_EYE_TIME_CALIBRATION_H_ */

