#ifndef DOVE_EYE_CHESSBOARD_PATTERN_H_
#define DOVE_EYE_CHESSBOARD_PATTERN_H_

#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_pattern.h"

namespace dove_eye {

class ChessboardPattern : public CalibrationPattern {
 public:
  ChessboardPattern(const int rows, const int columns, const double squareSize);

  bool Match(const cv::Mat &image, Point2Vector *points) const override;

  const Point3Vector & ObjectPoints() const override {
    return object_points_;
  }

 private:
  Point3Vector object_points_;
  cv::Size size_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CHESSBOARD_PATTERN_H_

