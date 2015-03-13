#ifndef DOVE_EYE_FRAMESET_AGGREGATOR_H_
#define DOVE_EYE_FRAMESET_AGGREGATOR_H_

#include <queue>
#include <vector>

#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/frame_iterator.h"
#include "dove_eye/video_provider.h"


namespace dove_eye {


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
    explicit Iterator(Aggregator *aggregator = nullptr, const bool valid = true)
        : aggregator_(aggregator),
          valid_(valid && aggregator && aggregator->width() > 0),
          window_start_(0),
          queues_(aggregator ? aggregator->width() : 0),
          frameset_(aggregator ? aggregator->width() : 0) {
      if (aggregator_) {
        aggregator_->frame_reader_.Start();
      }
    }

    explicit Iterator(const CameraIndex width)
        : aggregator_(nullptr),
          valid_(false),
          window_start_(0),
          queues_(0),
          frameset_(width) {
    }

    Frameset operator*() const {
      return frameset_;
    }

    Iterator &operator++() {
      Frame frame;
      CameraIndex cam;
      if (valid_ && !aggregator_->frame_reader_.GetFrame(&frame, &cam)) {
        valid_ = false;
        return *this;
      }

      /*
       * Apply offset,
       * see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
       */
      frame.timestamp -= aggregator_->offsets_[cam];

      queues_[cam].push(frame);

      if (frame.timestamp > window_start_ + aggregator_->window_size_) {
        window_start_ = frame.timestamp - aggregator_->window_size_;
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

    Aggregator *aggregator_;
    bool valid_;
    Frame::Timestamp window_start_;
    QueuesContainer queues_;
    Frameset frameset_;

    void PrepareFrameset() {
      for (CameraIndex cam = 0; cam < aggregator_->width(); ++cam) {
        Frame last_frame;
        bool has_frame = false;
        while (!queues_[cam].empty() &&
               queues_[cam].front().timestamp < window_start_) {
          last_frame = queues_[cam].front();
          has_frame = true;
          queues_[cam].pop();
        }

        if (has_frame) {
          frameset_.SetValid(cam);
          frameset_[cam] = last_frame;
        } else {
          /*
           * FIXME Think about this, set to invalid, however, last frame still
           * accessible.
           */
          frameset_.SetValid(cam, false);
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

