#ifndef DOVE_EYE_FRAMESET_H_
#define DOVE_EYE_FRAMESET_H_

#include "dove_eye/frame.h"
#include "dove_eye/tuple.h"

namespace dove_eye {

typedef Tuple<Frame> Frameset;

} // namespace dove_eye

#ifdef HAVE_GUI
/*
 * Make Frameset available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Frameset)
#endif

#endif // DOVE_EYE_FRAMESET_H_

