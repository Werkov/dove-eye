#ifndef DOVE_EYE_KALMAN_FILTER_H_
#define DOVE_EYE_KALMAN_FILTER_H_

#include <cassert>

namespace dove_eye {

/** False Kalman filter
 *
 * Naive implementation that only assumes constant velocity and ignores any
 * noise at all.
 */
template <typename T>
class KalmanFilter {
 public:
  KalmanFilter() {
    Reset();
  }

  T Predict(const double time) const {
    auto delta = time - prev_time_;
    return prev_state_ + derivative_ * delta;
  }

  void Update(const double time, const T observation) {
    if (prev_time_ == 0) {
      Reset(time, observation);
      return;
    }

    auto delta = time - prev_time_;
    assert(delta > 0);
    derivative_ = (observation - prev_state_) * (1 / delta);
    prev_state_ = observation;
    prev_time_ = time;
  }

  void Reset(const double time = 0, const T observation = T()) {
    derivative_ = T();
    prev_time_ = time;
    prev_state_ = observation;
  }

 private:
  double prev_time_;
  T prev_state_;
  T derivative_;
};

} // namespace dove_eye

#endif // DOVE_EYE_KALMAN_FILTER_H_

