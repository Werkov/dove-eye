#include "dove_eye/time_calibration.h"

#include <cassert>

namespace dove_eye {

bool TimeCalibration::MeasureFrame(const Frame &frame) {
  Frame::Timestamp observed = DetectTime(frame.data);
  if (observed < 0) {
    return false;
  }
  processingSum_ += frame.timestamp - observed;
  frames_ += 1;

  state_ = (frames_ >= framesToCollect_) ? kReady : kCollecting;
  return state_ == kReady;
}

void TimeCalibration::Reset() {
  state_ = kUnitialized;
  frames_ = 0;
  processingSum_ = 0;
}

TimeCalibration::ResultType TimeCalibration::result() {
  assert(state_ == kReady);
  return processingSum_ / frames_;
}

Frame::Timestamp TimeCalibration::DetectTime(const cv::Mat& data) {
  return -1; // TODO implement
}
}
