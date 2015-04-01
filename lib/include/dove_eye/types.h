#ifndef DOVE_EYE_TYPES_H_
#define DOVE_EYE_TYPES_H_

#include <vector>
#include <opencv2/opencv.hpp>


namespace dove_eye {

typedef std::vector<cv::Point2f> Point2Vector;
typedef std::vector<cv::Point3f> Point3Vector;

typedef int CameraIndex;

struct CameraParameters {
  cv::Mat camera_matrix;
  cv::Mat distortion_coefficients;
};

struct PairParameters {
  cv::Mat essential_matrix;
};


};

#ifdef HAVE_GUI
/*
 * Make CameraIndex available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::CameraIndex)
#endif


#endif // DOVE_EYE_TYPES_H_

