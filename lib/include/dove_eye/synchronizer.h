#ifndef DOVE_EYE_SYNCHRONIZER_H_
#define	DOVE_EYE_SYNCHRONIZER_H_

#include <opencv2/opencv.hpp>

#include <dove_eye/frame.h>

/**
 * \see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
 */
namespace dove_eye {

class SynchronizerIterator {
};

class Synchronizer {
 public:
  SynchronizerIterator begin();
  SynchronizerIterator &end();
 private:
  SynchronizerIterator end_;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_SYNCHRONIZER_H_ */

