#include "dove_eye/localization.h"

#include <opencv2/opencv.hpp>

#include "dove_eye/logging.h"
#include "dove_eye/types.h"

using cv::triangulatePoints;

namespace dove_eye {

bool Localization::Locate(const Positset &positset, Location *result) {
  assert(positset.Arity() == Arity());
  assert(result);

  //TODO verify it's initialized to zeroes
  Location location;
  size_t used_pairs = 0;

  for (auto pair : pairs_) {
    if (!positset.IsValid(pair.cam1) || !positset.IsValid(pair.cam2)) {
      continue;
    }

    Location pair_location = PairLocate(positset, pair);

    /*
     * Naive method that estimates real position as a centroid of individual
     * positions.
     */
    location += pair_location;
    ++used_pairs;
  }

  if (!used_pairs) {
    return false;
  }

  *result = location * (1.0 / used_pairs);
  return true;
}

Location Localization::PairLocate(const Positset &positset,
                                  const CameraPair pair) {
  assert(positset.IsValid(pair.cam1));
  assert(positset.IsValid(pair.cam2));
  assert(calibration_data_);

  auto p1 = calibration_data_->ProjectionMatrix(pair.cam1);
  Point2 point1(positset[pair.cam1]);
  cv::Mat points1(point1);

  auto p2 = calibration_data_->ProjectionMatrix(pair.cam2);
  Point2 point2(positset[pair.cam2]);
  cv::Mat points2(point2);

  /* BEWARE: triangulatePoints does create output matrix with same type as
   * points1 matrix (which is currenty float) */
  cv::Mat points_3d(4, 1, points1.type());

  triangulatePoints(p1, p2, points1, points2, points_3d);

  Point3 result(points_3d.rowRange(0, 3));
  /* Use a vector to obtain type-safe indexed type (at<T> is not typesafe) */
  cv::Vec4d aux_result(points_3d);
  result = result * (1 / aux_result[3]);

  return result;
}

} // namespace dove_eye
