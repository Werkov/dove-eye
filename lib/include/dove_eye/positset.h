#ifndef DOVE_EYE_POSITSET_H_
#define DOVE_EYE_POSITSET_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/tuple.h"

namespace dove_eye {

typedef Tuple<cv::Point2f> Positset;
;

} // namespace dove_eye

#ifdef HAVE_GUI
/*
 * Make Frameset available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Positset)
#endif

#endif // DOVE_EYE_POSITSET_H_

