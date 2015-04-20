#ifndef DOVE_EYE_FRAME_ITERATOR_CLOCK_POLICY_H_
#define DOVE_EYE_FRAME_ITERATOR_CLOCK_POLICY_H_


#include <chrono>

/* Forward declaration */
namespace cv {
class VideoCapture;
}


namespace dove_eye {
namespace frame_iterator {

class ClockPolicy {
 public:
  inline void Initialize(cv::VideoCapture *capture) {
    start_ = Clock::now();
  }

  inline double GetTimestamp() {
    auto now = Clock::now();
    std::chrono::duration<double> duration(now - start_);
    return duration.count();
  }

 private:
  typedef std::chrono::steady_clock Clock;
  typedef decltype(Clock::now()) TimePoint;

  TimePoint start_;
};

} // namespace frame_iterator
} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_CLOCK_POLICY_H_

