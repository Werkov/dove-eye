#ifndef DOVE_EYE_FRAME_H_
#define DOVE_EYE_FRAME_H_

#include <cstdint>

#include <opencv2/opencv.hpp>

namespace dove_eye {

struct Frame {
  typedef double Timestamp;
  typedef double TimestampDiff;

  Timestamp timestamp;
  cv::Mat data;
};

} // namespace dove_eye

#endif // DOVE_EYE_FRAME_H_

