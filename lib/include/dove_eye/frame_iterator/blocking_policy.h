#ifndef DOVE_EYE_FRAME_ITERATOR_BLOCKING_POLICY_H_
#define DOVE_EYE_FRAME_ITERATOR_BLOCKING_POLICY_H_

#include <cassert>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "dove_eye/video_provider.h"

namespace dove_eye {
namespace frame_iterator {

class BlockingPolicy {
 public:
  inline void Initialize(cv::VideoCapture *capture) {
    assert(capture->get(CV_CAP_PROP_FPS) > 0);
    frame_period_ = 1000000 / capture->get(CV_CAP_PROP_FPS);
  }

  inline void Wait() {
      std::this_thread::sleep_for(
        std::chrono::microseconds(frame_period_));
  }

 private:
  size_t frame_period_;
};

} // namespace frame_iterator
} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_BLOCKING_POLICY_H_

