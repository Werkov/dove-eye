#ifndef DOVE_EYE_AGGREGATOR_H_
#define DOVE_EYE_AGGREGATOR_H_

#include <cassert>
#include <deque>
#include <vector>

#include "dove_eye/aggregator_iterator.h"
#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frame_iterator.h"
#include "dove_eye/logging.h"
#include "dove_eye/parameters.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {


class Aggregator {
 public:
  friend class AggregatorIterator;
  typedef std::vector<VideoProvider *> ProvidersContainer;
  typedef AggregatorIterator Iterator;

  /**
   * @note FramesetAggregator takes ownership of contained video providers
   */
  Aggregator(const ProvidersContainer &providers,
             const dove_eye::Parameters &parameters)
      : arity_(providers.size()),
        parameters_(parameters),
        providers_(providers) {
  }

  virtual ~Aggregator() {
    for (auto provider_ptr : providers_) {
      delete provider_ptr;
    }
  }

  inline Iterator begin() {
    return Iterator(this);
  }

  inline Iterator end() {
    return Iterator(this, false);
  }

  inline CameraIndex Arity() const {
    return arity_;
  }

  inline const ProvidersContainer &providers() const {
    return providers_;
  }

  inline const Parameters &parameters() const {
    return parameters_;
  }

 private:
  CameraIndex arity_;
  const Parameters &parameters_;
  ProvidersContainer providers_;

  virtual void Start() = 0;

  virtual bool GetFrame(Frame *frame, CameraIndex *cam) = 0;
};

} // namespace dove_eye

#endif // DOVE_EYE_AGGREGATOR_H_

