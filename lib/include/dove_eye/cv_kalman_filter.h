#ifndef DOVE_EYE_CV_KALMAN_FILTER_H_
#define DOVE_EYE_CV_KALMAN_FILTER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/types.h"

namespace dove_eye {


class CvKalmanFilter {
 public:
  CvKalmanFilter()
      : kalman_filter_(4, 2, 0),
        prediction_valid_(false) {
  }

  CvKalmanFilter(const CvKalmanFilter &other);

  void Init(const double process_var, const double observation_var);

  Point2 Predict(const double time);

  Point2 PredictChange(const double time);

  Point2 Update(const double time, const Point2 observation);

  Point2 Reset(const double time = 0, const Point2 observation = Point2());

 private:
  typedef float MatType;

  cv::KalmanFilter kalman_filter_;
  cv::Mat prediction_;
  bool prediction_valid_;

  cv::Mat CreateTransition(const double delta) const;

  void RefreshPrediction();
};

} // namespace dove_eye

#endif // DOVE_EYE_CV_KALMAN_FILTER_H_

