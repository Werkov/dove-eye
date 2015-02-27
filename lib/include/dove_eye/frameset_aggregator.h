#ifndef DOVE_EYE_FRAMESET_AGGREGATOR_H_
#define DOVE_EYE_FRAMESET_AGGREGATOR_H_

#include <memory>
#include <queue>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frame_iterator.h"


namespace dove_eye {

class BlockingPolicy {
 public:
  typedef std::vector<std::unique_ptr<VideoProvider>> ProvidersContainer;

  explicit BlockingPolicy(ProvidersContainer &&providers)
      : providers_(std::move(providers)),
        current_cam_(0),
        initialized_(false),
        iterators_(providers_.size()),
        ends_(providers_.size()) {
  }

  bool GetFrame(Frame *frame, CameraIndex *cam) {
    if (!initialized_) {
      for (int i = 0; i < providers_.size(); ++i) {
        iterators_[i] = providers_[i]->begin();
        ends_[i] = providers_[i]->end();
      }
      initialized_ = true;
    }

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

template<class FramePolicy>
class FramesetAggregator {
 public:
  typedef std::vector<Frame::TimestampDiff> OffsetsContainer;

  /*
   * Iteration state is kept in iterator, however
   * during its increment underlying video providers are modified thus
   * invalidating any other iterators to the same FramesetAggregator.
   */
  class Iterator {
    typedef FramesetAggregator<FramePolicy> Aggregator;

   public:
    explicit Iterator(Aggregator *aggregator, bool valid = true)
        : aggregator_(*aggregator),
          valid_(valid),
          window_start_(0),
          queues_(aggregator->width()),
          frameset_(aggregator->width()) {
    }

    Frameset operator*() const {
      return frameset_;
    }

    Iterator &operator++() {
      Frame frame;
      CameraIndex cam;
      if (!aggregator_.frame_reader_.GetFrame(&frame, &cam)) {
        valid_ = false;
        return *this;
      }

      /*
       * Apply offset,
       * see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
       */
      frame.timestamp -= aggregator_.offsets_[cam];

      queues_[cam].push(frame);

      if (frame.timestamp > window_start_ + aggregator_.window_size_) {
        window_start_ = frame.timestamp - aggregator_.window_size_;
        PrepareFrameset();
      }

      return *this;
    }

    bool operator==(const Iterator &rhs) const {
      return !valid_ && !rhs.valid_;
    }

    inline bool operator!=(const Iterator &rhs) const {
      return !operator==(rhs);
    }

   private:
    typedef std::queue<Frame> FrameQueue;
    typedef std::vector<FrameQueue> QueuesContainer;

    Aggregator &aggregator_;
    bool valid_;
    Frame::Timestamp window_start_;
    QueuesContainer queues_;
    Frameset frameset_;

    void PrepareFrameset() {
      for (int i = 0; i < aggregator_.width(); ++i) {
        Frame last_frame;
        bool has_frame = false;
        while (!queues_[i].empty() &&
               queues_[i].front().timestamp < window_start_) {
          last_frame = queues_[i].front();
          has_frame = true;
          queues_[i].pop();
        }

        if (has_frame) {
          frameset_.SetValid(i);
          frameset_[i] = last_frame;
        } else {
          /*
           * FIXME Think about this, set to invalid, however, last frame still
           * accessible.
           */
          frameset_.SetValid(i, false);
        }
      }
    }
  };

  FramesetAggregator(typename FramePolicy::ProvidersContainer &&providers,
                     const OffsetsContainer &offsets,
                     Frame::TimestampDiff window_size)
      : frame_reader_(std::move(providers)),
        offsets_(offsets),
        window_size_(window_size) {
  }

  Iterator begin() {
    return Iterator(this);
  }

  Iterator end() {
    return Iterator(this, false);
  }

  CameraIndex width() const {
    return offsets_.size();
  }

 private:
  FramePolicy frame_reader_;
  OffsetsContainer offsets_;
  Frame::TimestampDiff window_size_;
};

} // namespace dove_eye

#endif // DOVE_EYE_FRAMESET_AGGREGATOR_H_

