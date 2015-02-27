#ifndef DOVE_EYE_FRAME_ITERATOR_H_
#define DOVE_EYE_FRAME_ITERATOR_H_

#include <memory>

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"

namespace dove_eye {

class FrameIteratorImpl {
 public:
  virtual ~FrameIteratorImpl() {}

  virtual Frame GetFrame() const = 0;

  virtual void MoveNext() = 0;

  virtual bool IsValid() = 0;
};

class FrameIterator {
 public:
  /**
   * Takes ownership of the iterator and it's shared among copies of the
   * iterator.
   */
  explicit FrameIterator(FrameIteratorImpl *iterator = nullptr)
      : iterator_(iterator) {
  }

  Frame operator*() const {
    return iterator_->GetFrame();
  }

  FrameIterator &operator++() {
    iterator_->MoveNext();
    return *this;
  }

  /**
   * Iterators equal only when they're both invalid.
   */
  bool operator==(const FrameIterator &rhs) {
    bool this_valid = iterator_ && iterator_->IsValid();
    bool rhs_valid = rhs.iterator_ && rhs.iterator_->IsValid();
    return !this_valid && !rhs_valid;
  }

  inline bool operator!=(const FrameIterator &rhs) {
    return !operator==(rhs);
  }

 private:
  std::shared_ptr<FrameIteratorImpl> iterator_;
};

} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_H_

