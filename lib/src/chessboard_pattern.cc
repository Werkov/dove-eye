#include "dove_eye/chessboard_pattern.h"

using cv::findChessboardCorners;

namespace dove_eye {

ChessboardPattern::ChessboardPattern(const int rows, const int columns,
                                     const double squareSize) :
 objectPoints_(rows * columns),
 size_(columns, rows) {
  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < columns; ++col) {
      // FIXME check transposition of coords and zeroness of z
      objectPoints_[col + row * columns].x = squareSize * col;
      objectPoints_[col + row * columns].y = squareSize * row;
    }
  }
}

bool ChessboardPattern::Match(const cv::Mat &image, Point2Vector &points) const {
  bool result = findChessboardCorners(image, size_, points,
    cv::CALIB_CB_ADAPTIVE_THRESH | 
    cv::CALIB_CB_NORMALIZE_IMAGE | 
    cv::CALIB_CB_FAST_CHECK);

  // TODO consider refinement with cornerSubPix
  return result;
}


} // namespace dove_eye


