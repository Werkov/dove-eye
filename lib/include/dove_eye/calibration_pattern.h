#ifndef DOVE_EYE_CALIBRATION_PATTERN_H_
#define	DOVE_EYE_CALIBRATION_PATTERN_H_

#include <vector>

#include <opencv2/opencv.hpp>

#include <dove_eye/types.h>

namespace dove_eye {

class CalibrationPattern {
 public:
  virtual bool Match(const cv::Mat &image, Point2Vector &points) const = 0;

  virtual const Point3Vector & ObjectPoints() const = 0;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_CALIBRATION_PATTERN_H_ */

