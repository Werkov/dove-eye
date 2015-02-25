#ifndef DOVE_EYE_FRAMESET_H_
#define	DOVE_EYE_FRAMESET_H_

#include <cassert>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include <dove_eye/frame.h>
#include <dove_eye/types.h>

namespace dove_eye {

/*
 * Represents set of frames of the scene for each camera.
 * Not all camera frames may be present.
 *
 * \note Limited to kMaxSize cameras.
 */
class Frameset {
 public:
  static const CameraIndex kMaxSize = 4;

  Frameset() = delete;

  Frameset(const CameraIndex size) :
   size_(size),
   validity_() {
    assert(size_ <= kMaxSize);
  }

  inline Frame &operator[](const CameraIndex cam) {
    assert(cam < size_);
    return frames_[cam];
  }

  inline const Frame &operator[](const CameraIndex cam) const {
    assert(cam < size_);
    return frames_[cam];
  }

  inline void SetValid(const CameraIndex cam, const bool value = true) {
    assert(cam < size_);
    validity_[cam] = value;
  }

  inline bool IsValid(const CameraIndex cam) const {
    assert(cam < size_);
    return validity_[cam];
  }

  inline CameraIndex Size() const {
    return size_;
  }

 private:
  const CameraIndex size_;
  Frame frames_[kMaxSize];
  bool validity_[kMaxSize];
};

} // namespace dove_eye

#endif	/* DOVE_EYE_FRAMESET_H_ */

