#ifndef DOVE_EYE_BLOCKING_POLICY_H_
#define DOVE_EYE_BLOCKING_POLICY_H_

#include <cassert>
#include <memory>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/types.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {

class BlockingPolicy {
 public:
  typedef std::vector<VideoProvider *> ProvidersContainer;

  explicit BlockingPolicy(const ProvidersContainer &providers)
      : providers_(providers),
        current_cam_(0),
        initialized_(false),
        iterators_(providers_.size()),
        ends_(providers_.size()) {
  }

  void Start() {
    if (!initialized_) {
      if (providers_.size() == 0) {
        /* Not set initalized_, next time make this check too. */
        return;
      }

      for (int i = 0; i < providers_.size(); ++i) {
        iterators_[i] = providers_[i]->begin();
        ends_[i] = providers_[i]->end();
      }
      initialized_ = true;
    }
  }

  bool GetFrame(Frame *frame, CameraIndex *cam) {
    assert(initialized_);

    int attempts = 0;
    while (iterators_[current_cam_] == ends_[current_cam_] &&
           attempts < providers_.size()) {
      current_cam_ = (current_cam_ + 1) % providers_.size();
      attempts += 1;
    }

    /* All providers finished. */
    if (attempts == providers_.size()) {
      return false;
    }

    *frame = *iterators_[current_cam_];
    *cam = current_cam_;

    ++iterators_[current_cam_];
    /* round-robin on cameras */
    current_cam_ = (current_cam_ + 1) % providers_.size();

    return true;
  }

 private:
  typedef std::vector<FrameIterator> Iterators;

  ProvidersContainer providers_;
  CameraIndex current_cam_;
  bool initialized_;
  Iterators iterators_;
  Iterators ends_;
};


} // namespace dove_eye

#endif // DOVE_EYE_BLOCKING_POLICY_H_

