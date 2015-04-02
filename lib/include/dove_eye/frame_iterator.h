#ifndef DOVE_EYE_FRAME_ITERATOR_H_
#define DOVE_EYE_FRAME_ITERATOR_H_

#include <memory>

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"

namespace dove_eye {

/* Forward */
class VideoProvider;

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
  explicit FrameIterator(const VideoProvider *video_provider = nullptr,
                FrameIteratorImpl *iterator = nullptr)
      : video_provider_(video_provider),
        iterator_(iterator),
        is_frame_valid_(true) {
  }

  Frame operator*() const;

  FrameIterator &operator++();

  /**
   * Iterators equal only when they're both invalid.
   */
  inline bool operator==(const FrameIterator &rhs) {
    bool this_valid = iterator_ && iterator_->IsValid();
    bool rhs_valid = rhs.iterator_ && rhs.iterator_->IsValid();
    return !this_valid && !rhs_valid;
  }

  inline bool operator!=(const FrameIterator &rhs) {
    return !operator==(rhs);
  }

 private:
  const VideoProvider *video_provider_;
  std::shared_ptr<FrameIteratorImpl> iterator_;

  mutable bool is_frame_valid_;
  mutable Frame frame_;
};

} // namespace dove_eye

#endif // DOVE_EYE_FRAME_ITERATOR_H_

