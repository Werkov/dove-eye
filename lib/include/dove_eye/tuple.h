#ifndef DOVE_EYE_TUPLE_H_
#define DOVE_EYE_TUPLE_H_

#include <algorithm>
#include <cassert>
#include <cstdint>


#include "dove_eye/types.h"

namespace dove_eye {

/*
 * Represents a tuple of objects for each camera.
 * Not all objects may be present.
 *
 * \note Limited to kMaxSize cameras.
 */
template<typename T>
class Tuple {
 public:
  typedef T value_type;
  typedef T *iterator;
  typedef const T *const_iterator;

  static const CameraIndex kMaxSize = 4;

  /*
   * Because of the const member size_ we have to provide the Big Five
   * methods for copying/moving.
   */
  explicit Tuple(const CameraIndex size = 0)
      : size_(size),
        validity_() {
    assert(size_ <= kMaxSize);
  }

  Tuple(const Tuple &other)
      : size_(other.size_) {
    assert(size_ == other.size_);

    for (CameraIndex cam = 0; cam < size_; ++cam) {
      items_[cam] = other.items_[cam];
      validity_[cam] = other.validity_[cam];
    }
  }

  Tuple(Tuple &&other)
      : size_(other.size_) {
    assert(size_ == other.size_);

    for (CameraIndex cam = 0; cam < size_; ++cam) {
      items_[cam] = std::move(other.items_[cam]);
      validity_[cam] = other.validity_[cam];
    }
  }

  inline Tuple &operator=(const Tuple &rhs) {
    Tuple tmp(rhs);
    *this = std::move(tmp);
    return *this;
  }

  inline Tuple &operator=(Tuple &&rhs) {
    assert(size_ == rhs.size_);

    std::swap(items_, rhs.items_);
    std::swap(validity_, rhs.validity_);

    return *this;
  }

  inline T &operator[](const CameraIndex cam) {
    assert(cam < size_);
    return items_[cam];
  }

  inline const T &operator[](const CameraIndex cam) const {
    assert(cam < size_);
    return items_[cam];
  }

  inline const_iterator begin() const {
    return items_;
  }

  inline const_iterator end() const {
    return items_ + size_;
  }

  inline iterator begin() {
    return items_;
  }

  inline iterator end() {
    return items_ + size_;
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
  T items_[kMaxSize];
  bool validity_[kMaxSize];
};

} // namespace dove_eye

#endif // DOVE_EYE_TUPLE_H_

