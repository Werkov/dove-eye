#ifndef DOVE_EYE_POSITSET_H_
#define DOVE_EYE_POSITSET_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/tuple.h"
#include "dove_eye/types.h"

namespace dove_eye {

/** Posit is output of inner tracker (i.e. processing single camera only)
 * @note 'posit' is a term on its own, consider it just a coincidence that
 *       it resembles word position (in image).
 */
typedef Point2 Posit;

/** Positset represents general aggregated output of tracker */
typedef Tuple<Point2> Positset;

} // namespace dove_eye

#ifdef HAVE_GUI
/*
 * Make Frameset available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Positset)
#endif

#endif // DOVE_EYE_POSITSET_H_

