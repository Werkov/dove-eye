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
    return prev_state_ + PredictChange(prev_time_) * delta;
  }

  T PredictChange(const double time) const {
    /* Linear approximation */
    return derivative_;
  }

  T Update(const double time, const T observation) {
    if (prev_time_ == 0) {
      Reset(time, observation);
      return observation;
    }

    auto delta = time - prev_time_;
    assert(delta > 0);
    derivative_ = (observation - prev_state_) * (1 / delta);
    prev_state_ = observation;
    prev_time_ = time;
    return observation;
  }

  T Reset(const double time = 0, const T observation = T()) {
    derivative_ = T();
    prev_time_ = time;
    prev_state_ = observation;
    return observation;
  }

 private:
  double prev_time_;
  T prev_state_;
  T derivative_;
};

} // namespace dove_eye

#endif // DOVE_EYE_KALMAN_FILTER_H_

