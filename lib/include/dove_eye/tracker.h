#ifndef DOVE_EYE_TRACKER_H_
#define DOVE_EYE_TRACKER_H_


#include "dove_eye/frameset.h"
#include "dove_eye/positset.h"

namespace dove_eye {

class Tracker {
 public:
  Positset Track(const Frameset &frameset);
};

} // namespace dove_eye

#endif // DOVE_EYE_TRACKER_H_

