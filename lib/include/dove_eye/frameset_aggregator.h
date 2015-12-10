#ifndef DOVE_EYE_FRAMESET_AGGREGATOR_H_
#define DOVE_EYE_FRAMESET_AGGREGATOR_H_

#include <cassert>
#include <deque>
#include <vector>

#include "dove_eye/aggregator.h"
#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frame_iterator.h"
#include "dove_eye/logging.h"
#include "dove_eye/parameters.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {


template<class FramePolicy>
class FramesetAggregator : public Aggregator {
 public:

  /**
   * @note FramesetAggregator takes ownership of contained video providers
   */
  FramesetAggregator(const ProvidersContainer &providers,
                     const dove_eye::Parameters &parameters)
      : Aggregator(providers, parameters),
        frame_policy_(providers) {

  }

  virtual void Start() override {
    frame_policy_.Start();
  }

  virtual bool GetFrame(Frame *frame, CameraIndex *cam) override {
    return frame_policy_.GetFrame(frame, cam);
  }

 private:
  /* Policy must follow Aggregator::parameters_ (because of destruction order) */
  FramePolicy frame_policy_;

};

} // namespace dove_eye

#endif // DOVE_EYE_FRAMESET_AGGREGATOR_H_

