#ifndef DOVE_EYE_FRAME_ITERATOR_NONBLOCKING_POLICY_H_
#define DOVE_EYE_FRAME_ITERATOR_NONBLOCKING_POLICY_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/video_provider.h"

namespace dove_eye {
namespace frame_iterator {

class NonblockingPolicy {
 public:
  inline void Initialize(cv::VideoCapture *capture) {
    /* empty */
  }

  inline void Wait() {
    /* empty */
  }
};

} // namespace frame_iterator
} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_NONBLOCKING_POLICY_H_

