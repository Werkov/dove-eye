#ifndef DOVE_EYE_TYPES_H_
#define DOVE_EYE_TYPES_H_

#include <vector>
#include <opencv2/opencv.hpp>


namespace dove_eye {

typedef std::vector<cv::Point2f> Point2Vector;
typedef std::vector<cv::Point3f> Point3Vector;

typedef int CameraIndex;

};

#endif // DOVE_EYE_TYPES_H_

