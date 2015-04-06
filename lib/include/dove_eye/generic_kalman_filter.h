#ifndef DOVE_EYE_KALMAN_FILTER_H_
#define DOVE_EYE_KALMAN_FILTER_H_

/* Just a draft of an interface */

namespace dove_eye {

template <typename T, typename Time = double>
class LinearPolicy {
 public:
  typedef T OuterState;
  typedef T Observation;
  typedef Time TimeT;

  struct State {
    T x;
    T x_dot;
    Time time;
  };

  struct Error {
    /** Row major order
     * (x_var, x_dot_var on main diagonal)*/
    T mat[4];
  };
  
  State Predict(const State &state, const Time time) const {
    auto delta = time - state.time;
    State result(state);
    result.x += state.x_dot * delta;
    /* x_dot is predicted constant */

    return result;
  }

  Error PredictError(const Error &error, const Time time) const {

  }

  OuterState Publish(const State &state) {
    return state.state;
  }



 private:
};

template <typename Policy>
class KalmanFilter {
 public:
  Policy::OuterState Predict(const Policy::Time time) {
    auto state = policy_.Predict(time);
    return policy_.Publish(state);
  }

  void Update(const Policy::Time time, const Policy::Observation observation) {
    auto pred_state = policy_.Predict(state_, time);
    auto pred_error = policy_.PredictError(error_, time);

    auto residual = observation - policy_.Observe(pred_state);
    auto gain = policy_.Gain(pred_state);

    state_ = policy_.UpdatedState(state_, gain, residual);
    error_ = policy_.UpdatedError(error_, gain);
  }
 private:
  Policy::State state_;
  Policy::Error error_;

  
};

} // namespace dove_eye

#endif // DOVE_EYE_KALMAN_FILTER_H_

