#ifndef DOVE_EYE_CV_CAPTURE_LOCK_H_
#define DOVE_EYE_CV_CAPTURE_LOCK_H_

#include <mutex>

namespace dove_eye {

/**
 * @see http://stackoverflow.com/a/12243521/1351874
 */
extern std::mutex cv_capture_mtx;

} // end namespace dove_eye


#endif // DOVE_EYE_CV_CAPTURE_LOCK_H_
