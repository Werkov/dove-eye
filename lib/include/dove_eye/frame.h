#ifndef DOVE_EYE_FRAME_H_
#define	DOVE_EYE_FRAME_H_

#include <cstdint>

#include <opencv2/opencv.hpp>

namespace DoveEye {

struct Frame {
  typedef uint32_t Timestamp;
  
  Timestamp timestamp;
  cv::Mat data;
};

} // namespace DoveEye

#endif	/* DOVE_EYE_FRAME_H_ */

