#ifndef DOVE_EYE_FRAMESET_H_
#define DOVE_EYE_FRAMESET_H_

#include <algorithm>
#include <cassert>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"
#include "dove_eye/types.h"

namespace dove_eye {

/*
 * Represents set of frames of the scene for each camera.
 * Not all camera frames may be present.
 *
 * \note Limited to kMaxSize cameras.
 */
class Frameset {
 public:
  typedef Frame *iterator;
  typedef const Frame *const_iterator;

  static const CameraIndex kMaxSize = 4;

  /*
   * Because of the const member size_ we have to provide the Big Five
   * methods for copying/moving.
   */

  explicit Frameset(const CameraIndex size = 0)
      : size_(size),
        validity_() {
    assert(size_ <= kMaxSize);
  }

  Frameset(const Frameset &other)
      : size_(other.size_) {
    assert(size_ == other.size_);

    for (CameraIndex cam = 0; cam < size_; ++cam) {
      frames_[cam] = other.frames_[cam];
      validity_[cam] = other.validity_[cam];
    }
  }

  Frameset(Frameset &&other)
      : size_(other.size_) {
    assert(size_ == other.size_);

    for (CameraIndex cam = 0; cam < size_; ++cam) {
      frames_[cam] = std::move(other.frames_[cam]);
      validity_[cam] = other.validity_[cam];
    }
  }

  inline Frameset &operator=(const Frameset &rhs) {
    Frameset tmp(rhs);
    *this = std::move(tmp);
    return *this;
  }

  inline Frameset &operator=(Frameset &&rhs) {
    assert(size_ == rhs.size_);

    std::swap(frames_, rhs.frames_);
    std::swap(validity_, rhs.validity_);

    return *this;
  }

  inline Frame &operator[](const CameraIndex cam) {
    assert(cam < size_);
    return frames_[cam];
  }

  inline const Frame &operator[](const CameraIndex cam) const {
    assert(cam < size_);
    return frames_[cam];
  }

  inline const_iterator begin() const {
    return frames_;
  }

  inline const_iterator end() const {
    return frames_ + size_;
  }

  inline iterator begin() {
    return frames_;
  }

  inline iterator end() {
    return frames_ + size_;
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

#ifdef HAVE_GUI
/*
 * Make Frameset available as argument for Qt's queued connections.
 */
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::Frameset)
#endif

#endif // DOVE_EYE_FRAMESET_H_

