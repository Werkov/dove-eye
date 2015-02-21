#ifndef DOVE_EYE_FRAMESET_H_
#define	DOVE_EYE_FRAMESET_H_

#include <cstdint>

#include <opencv2/opencv.hpp>

#include <dove_eye/frame.h>

namespace dove_eye {

typedef size_t CameraIndex;

struct Frameset {
  const CameraIndex size;
  Frame *frames;

  Frameset(const CameraIndex size) : size(size) {

  }

  inline Frame &operator[](const CameraIndex cam) {
    return frames[cam];
  }

  inline const Frame &operator[](const CameraIndex cam) const {
    return frames[cam];
  }
};

} // namespace dove_eye

#endif	/* DOVE_EYE_FRAMESET_H_ */

