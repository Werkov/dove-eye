#ifndef DOVE_EYE_FRAMESET_AGGREGATOR_H_
#define DOVE_EYE_FRAMESET_AGGREGATOR_H_

#include <cassert>
#include <deque>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frame_iterator.h"
#include "dove_eye/logging.h"
#include "dove_eye/parameters.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {


template<class FramePolicy>
class FramesetAggregator {
 public:
  /*
   * Iteration state is kept in iterator, however
   * during its increment underlying video providers are modified thus
   * potentially invalidating any other iterators to the same
   * FramesetAggregator.
   */
  class Iterator {
    typedef FramesetAggregator<FramePolicy> Aggregator;

   public:
    explicit Iterator(Aggregator *aggregator = nullptr, const bool valid = true)
        : aggregator_(aggregator),
          valid_(valid && aggregator && aggregator->Arity() > 0),
          window_start_(0),
          queues_(aggregator ? aggregator->Arity() : 0),
          frameset_(aggregator ? aggregator->Arity() : 0) {
      /* If it's begin iterator, start the reader */
      if (aggregator_ && valid) {
        aggregator_->frame_policy_.Start();
      }
    }

    explicit Iterator(const CameraIndex arity)
        : aggregator_(nullptr),
          valid_(false),
          window_start_(0),
          queues_(0),
          frameset_(arity) {
    }

    Frameset operator*() const {
      return frameset_;
    }

    Iterator &operator++() {
      Frame frame;
      CameraIndex cam;
      bool frameset_created = false;

      do {
        if (valid_ && !aggregator_->frame_policy_.GetFrame(&frame, &cam)) {
          valid_ = false;
          return *this;
        }

        /*
         * Apply offset,
         * see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
         */
        frame.timestamp -=
            aggregator_->parameters_.Get(Parameters::CAM_OFFSET, cam);

        queues_[cam].push_back(frame);

        auto window_size =
            aggregator_->parameters_.Get(Parameters::AGGREGATOR_WINDOW);

        /* Move the window forwards? */
        if (frame.timestamp > window_start_ + window_size) {
          window_start_ = frame.timestamp - window_size;
          frameset_created = PrepareFrameset();
        }
      } while (!frameset_created);

      return *this;
    }

    bool operator==(const Iterator &rhs) const {
      return !valid_ && !rhs.valid_;
    }

    inline bool operator!=(const Iterator &rhs) const {
      return !operator==(rhs);
    }

   private:
    typedef std::deque<Frame> FrameQueue;
    typedef std::vector<FrameQueue> QueuesContainer;

    Aggregator *aggregator_;
    bool valid_;
    Frame::Timestamp window_start_;
    QueuesContainer queues_;
    Frameset frameset_;

    bool PrepareFrameset() {
      bool frameset_created = false;
      for (CameraIndex cam = 0; cam < aggregator_->Arity(); ++cam) {
        Frame last_frame;
        bool has_frame = false;
        while (!queues_[cam].empty() &&
               queues_[cam].front().timestamp < window_start_) {
          last_frame = queues_[cam].front();
          has_frame = true;
          queues_[cam].pop_front();
        }

        if (has_frame) {
          frameset_.SetValid(cam);
          frameset_[cam] = last_frame;
          frameset_created = true;
        } else {
          frameset_.SetValid(cam, false);
        }

      }

      return frameset_created;
    }

  }; // end class Iterator

  typedef std::vector<VideoProvider *> ProvidersContainer;

  /**
   * @note FramesetAggregator takes ownership of contained video providers
   */
  FramesetAggregator(const ProvidersContainer &providers,
                     const dove_eye::Parameters &parameters)
      : arity_(providers.size()),
        parameters_(parameters),
        providers_(providers),
        frame_policy_(providers) {

  }

  ~FramesetAggregator() {
    for (auto provider_ptr : providers_) {
      delete provider_ptr;
    }
  }

  Iterator begin() {
    return Iterator(this);
  }

  Iterator end() {
    return Iterator(this, false);
  }

  CameraIndex Arity() const {
    return arity_;
  }

  const ProvidersContainer &providers() const {
    return providers_;
  }



 private:
  CameraIndex arity_;
  const Parameters &parameters_;
  ProvidersContainer providers_;
  /* Policy must follow parameters (because of destruction order) */
  FramePolicy frame_policy_;

};

} // namespace dove_eye

#endif // DOVE_EYE_FRAMESET_AGGREGATOR_H_

