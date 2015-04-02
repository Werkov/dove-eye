#include "dove_eye/frame_iterator.h"

#include "dove_eye/video_provider.h"

namespace dove_eye {

Frame FrameIterator::operator*() const {
  assert(video_provider_);

  if (!is_frame_valid_) {
    is_frame_valid_ = true;
    frame_ = iterator_->GetFrame();
    video_provider_->PreprocessFrame(&frame_);
  }
  return frame_;
}

FrameIterator &FrameIterator::operator++() {
  iterator_->MoveNext();
  is_frame_valid_ = false;
  return *this;
}

} // end namespace dove_eye
