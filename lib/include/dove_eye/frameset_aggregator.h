#ifndef DOVE_EYE_FRAMESET_AGGREGATOR_H_
#define	DOVE_EYE_FRAMESET_AGGREGATOR_H_

#include <memory>
#include <queue>
#include <vector>

#include <dove_eye/frame.h>
#include <dove_eye/frameset.h>
#include <dove_eye/frame_iterator.h>


namespace dove_eye {

class BlockingPolicy {
 public:
  typedef std::vector<std::unique_ptr<VideoProvider>> ProvidersContainer;

  BlockingPolicy(ProvidersContainer &&providers) :
   providers_(std::move(providers)),
   currentCam_(0),
   initialized_(false),
   iterators_(providers_.size()),
   ends_(providers_.size()) {
  }

  bool GetFrame(Frame &frame, CameraIndex &cam) {
    if (!initialized_) {
      for (int i = 0; i < providers_.size(); ++i) {
        iterators_[i] = providers_[i]->begin();
        ends_[i] = providers_[i]->end();
      }
      initialized_ = true;
    }
    
    int attempts = 0;
    while (iterators_[currentCam_] == ends_[currentCam_] &&
           attempts < providers_.size()) {
      currentCam_ = (currentCam_ + 1) % providers_.size();
      attempts += 1;
    }
    
    /* All providers finished. */
    if (attempts == providers_.size()) {
      return false;
    }

    frame = *iterators_[currentCam_];
    cam = currentCam_;

    ++iterators_[currentCam_];
    /* round-robin on cameras */
    currentCam_ = (currentCam_ + 1) % providers_.size();
    
    return true;
  }

 private:
  typedef std::vector<FrameIterator> Iterators;

  ProvidersContainer providers_;
  CameraIndex currentCam_;
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
    Iterator(Aggregator &aggregator, bool valid = true) :
     aggregator_(aggregator),
     valid_(valid),
     windowStart_(0),
     queues_(aggregator.width()),
     frameset_(aggregator.width()) {
    }

    Frameset operator*() const {
      return frameset_;
    }

    Iterator &operator++() {
      Frame frame;
      CameraIndex cam;
      if (!aggregator_.frameReader_.GetFrame(frame, cam)) {
        valid_ = false;
        return *this;
      }

      /* Apply offset, see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time */
      frame.timestamp -= aggregator_.offsets_[cam];

      queues_[cam].push(frame);

      if (frame.timestamp > windowStart_ + aggregator_.windowSize_) {
        windowStart_ = frame.timestamp - aggregator_.windowSize_;
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
    Frame::Timestamp windowStart_;
    QueuesContainer queues_;
    Frameset frameset_;

    void PrepareFrameset() {
      for(int i = 0; i < aggregator_.width(); ++i) {
        Frame lastFrame;
        bool hasFrame = false;
        while (!queues_[i].empty() &&
               queues_[i].front().timestamp < windowStart_) {
          lastFrame = queues_[i].front();
          hasFrame = true;
          queues_[i].pop();
        }

        if (hasFrame) {
          frameset_.SetValid(i);
          frameset_[i] = lastFrame;
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
                     Frame::TimestampDiff windowSize) :
   frameReader_(std::move(providers)),
   offsets_(offsets),
   windowSize_(windowSize) {
  }

  Iterator begin() {
    return Iterator(*this);
  }

  Iterator end() {
    return Iterator(*this, false);
  }

  CameraIndex width() const {
    return offsets_.size();
  }

 private:
  FramePolicy frameReader_;
  OffsetsContainer offsets_;
  Frame::TimestampDiff windowSize_;

};

} // namespace dove_eye

#endif	/* DOVE_EYE_FRAMESET_AGGREGATOR_H_ */

