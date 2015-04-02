#ifndef DOVE_EYE_LOCATION_H_
#define DOVE_EYE_LOCATION_H_

#include "dove_eye/types.h"

namespace dove_eye {

typedef Point3 Location;

} // namespace dove_eye

#ifdef HAVE_GUI
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Location)
#endif

#endif // DOVE_EYE_LOCATION_H_

