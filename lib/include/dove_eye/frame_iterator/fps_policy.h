#ifndef DOVE_EYE_FRAME_ITERATOR_FPS_POLICY_H_
#define DOVE_EYE_FRAME_ITERATOR_FPS_POLICY_H_


#include <opencv2/opencv.hpp>


namespace dove_eye {
namespace frame_iterator {

class FpsPolicy {
 public:
  inline void Initialize(cv::VideoCapture *capture) {
    assert(capture->get(CV_CAP_PROP_FPS) > 0);
    frame_period_ = 1.0 / capture->get(CV_CAP_PROP_FPS);
    frame_no_ = 0;
  }

  inline double GetTimestamp() {
    return (++frame_no_) * frame_period_;
  }

 private:
  double frame_period_;
  size_t frame_no_;
};

} // namespace frame_iterator
} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_FPS_POLICY_H_

