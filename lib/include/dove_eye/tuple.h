#ifndef DOVE_EYE_TUPLE_H_
#define DOVE_EYE_TUPLE_H_

#include <algorithm>
#include <cassert>
#include <cstdint>

#include "config.h"
#include "dove_eye/types.h"

namespace dove_eye {

/*
 * Represents a tuple of objects for each camera.
 * Not all objects may be present.
 *
 * \note Limited to kMaxArity cameras.
 */
template<typename T>
class Tuple {
 public:
  typedef T value_type;
  typedef T *iterator;
  typedef const T *const_iterator;

  static const CameraIndex kMaxArity = CONFIG_MAX_ARITY;

  /*
   * Because of the const member arity_ we have to provide the Big Five
   * methods for copying/moving.
   */
  explicit Tuple(const CameraIndex size = 0)
      : arity_(size),
        validity_() {
    assert(arity_ <= kMaxArity);
  }

  Tuple(const Tuple &other)
      : arity_(other.arity_) {
    assert(arity_ == other.arity_);

    for (CameraIndex cam = 0; cam < arity_; ++cam) {
      items_[cam] = other.items_[cam];
      validity_[cam] = other.validity_[cam];
    }
  }

  Tuple(Tuple &&other)
      : arity_(other.arity_) {
    assert(arity_ == other.arity_);

    for (CameraIndex cam = 0; cam < arity_; ++cam) {
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
    assert(arity_ == rhs.arity_);

    std::swap(items_, rhs.items_);
    std::swap(validity_, rhs.validity_);

    return *this;
  }

  inline T &operator[](const CameraIndex cam) {
    assert(cam < arity_);
    return items_[cam];
  }

  inline const T &operator[](const CameraIndex cam) const {
    assert(cam < arity_);
    return items_[cam];
  }

  inline const_iterator begin() const {
    return items_;
  }

  inline const_iterator end() const {
    return items_ + arity_;
  }

  inline iterator begin() {
    return items_;
  }

  inline iterator end() {
    return items_ + arity_;
  }

  inline void SetValid(const CameraIndex cam, const bool value = true) {
    assert(cam < arity_);
    validity_[cam] = value;
  }

  inline bool IsValid(const CameraIndex cam) const {
    assert(cam < arity_);
    return validity_[cam];
  }

  inline CameraIndex ValidCount() const {
    CameraIndex result = 0;
    for (CameraIndex cam = 0; cam < arity_; ++cam) {
      result += validity_[cam] ? 1 : 0;
    }
    return result;
  }

  inline CameraIndex Arity() const {
    return arity_;
  }

 private:
  const CameraIndex arity_;
  T items_[kMaxArity];
  bool validity_[kMaxArity];
};

} // namespace dove_eye

#endif // DOVE_EYE_TUPLE_H_

