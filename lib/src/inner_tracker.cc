#include "dove_eye/inner_tracker.h"

namespace dove_eye {

cv::Mat InnerTracker::EpilineToMask(const cv::Size size,
                                    const int thickness,
                                    const Epiline epiline) const {
  /*
   * Epiline:
   *    a*x + b*y + c = 0
   *
   *    y = (a*x + c) / -b
   *    x = (b*y + c) / -a
   */

  Point2 p1;
  Point2 p2;
  if (std::abs(epiline[1]) > std::abs(epiline[0])) {
    p1.x = 0;
    p1.y = (epiline[0] * p1.x + epiline[2]) / -epiline[1];

    p2.x = size.width;
    p2.y = (epiline[0] * p2.x + epiline[2]) / -epiline[1];
  } else {
    p1.y = 0;
    p1.x = (epiline[1] * p1.y + epiline[2]) / -epiline[0];

    p2.y = size.height;
    p2.x = (epiline[1] * p2.y + epiline[2]) / -epiline[0];
  }

  cv::Mat mask(size, CV_8U);
  mask.setTo(cv::Scalar(0, 0, 0));


  line(mask, p1, p2, cv::Scalar(255, 255, 255), thickness);

  return mask;
}

} // end namespace dove_eye
