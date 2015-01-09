#ifndef DOVE_EYE_TIME_CALIBRATION_H_
#define	DOVE_EYE_TIME_CALIBRATION_H_

#include <dove_eye/frame.h>

namespace DoveEye {

class TimeCalibration {
 public:
  typedef Frame::Timestamp ValueType;

  bool MeasureFrame(const Frame &frame);

  void Reset();

  ValueType result();
 private:
  ValueType result_;
};

} // namespace DoveEye

#endif	/* DOVE_EYE_TIME_CALIBRATION_H_ */

