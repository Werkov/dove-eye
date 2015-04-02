#ifndef DOVE_EYE_TYPES_H_
#define DOVE_EYE_TYPES_H_

#include <vector>
#include <opencv2/opencv.hpp>


namespace dove_eye {

typedef cv::Point2f Point2;
typedef cv::Point3f Point3;

typedef std::vector<Point2> Point2Vector;
typedef std::vector<Point3> Point3Vector;

typedef int CameraIndex;

};

#ifdef HAVE_GUI
/*
 * Make CameraIndex available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::CameraIndex)
#endif


#endif // DOVE_EYE_TYPES_H_

