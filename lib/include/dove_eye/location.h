#ifndef DOVE_EYE_LOCATION_H_
#define DOVE_EYE_LOCATION_H_


namespace dove_eye {

struct Location {
  double x;
  double y;
  double z;
};

} // namespace dove_eye

#ifdef HAVE_GUI
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Location)
#endif

#endif // DOVE_EYE_LOCATION_H_

