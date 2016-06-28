#include "dove_eye/cv_kalman_filter.h"

using cv::Scalar;

namespace dove_eye {

/**
 * cv::Mat predicted state cannot be shared!
 */
CvKalmanFilter::CvKalmanFilter(const CvKalmanFilter &other)
    : kalman_filter_(other.kalman_filter_),
      prediction_(other.prediction_.clone()),
      prediction_valid_(other.prediction_valid_) {

  /* This should force creation of new (unshared) state matrices */
  kalman_filter_ = cv::KalmanFilter(4, 2, 0);
}

void CvKalmanFilter::Init(const double process_var, const double observation_var) {
  
  kalman_filter_.transitionMatrix = (cv::Mat_<MatType>(4, 4) <<
                                      1, 0, 1, 0,
                                      0, 1, 0, 1,
                                      0, 0, 1, 0,
                                      0, 0, 0, 1);
  setIdentity(kalman_filter_.measurementMatrix);
  setIdentity(kalman_filter_.processNoiseCov, Scalar::all(process_var));
  setIdentity(kalman_filter_.measurementNoiseCov, Scalar::all(observation_var));

  Reset();
}

Point2 CvKalmanFilter::Predict(const double time) {
  if (!prediction_valid_) {
    RefreshPrediction();
  }

  return Point2(prediction_.at<MatType>(0, 0),
                prediction_.at<MatType>(1, 0));
}

Point2 CvKalmanFilter::PredictChange(const double time) {
  if (!prediction_valid_) {
    RefreshPrediction();
  }

  return Point2(prediction_.at<MatType>(2, 0),
                prediction_.at<MatType>(3, 0));
}

Point2 CvKalmanFilter::Update(const double time, const Point2 observation) {
  cv::Mat_<MatType> mat_observation(2, 1);
  mat_observation(0) = observation.x;
  mat_observation(1) = observation.y;

  cv::Mat estimate = kalman_filter_.correct(mat_observation);
  prediction_valid_ = false;

  return Point2(estimate.at<MatType>(0, 0),
                estimate.at<MatType>(1, 0));
}

Point2 CvKalmanFilter::Reset(const double time, const Point2 observation) {
  setIdentity(kalman_filter_.errorCovPost, Scalar::all(1));

  kalman_filter_.statePost.at<MatType>(0) = observation.x;
  kalman_filter_.statePost.at<MatType>(1) = observation.y;
  kalman_filter_.statePost.at<MatType>(2) = 0;
  kalman_filter_.statePost.at<MatType>(3) = 0;

  return observation;
}


void CvKalmanFilter::RefreshPrediction() {
  prediction_ = kalman_filter_.predict();
  prediction_valid_ = true;
}

} // end namespace dove_eye
