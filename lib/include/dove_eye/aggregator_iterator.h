#ifndef DOVE_EYE_AGGREGATOR_ITERATOR_H_
#define DOVE_EYE_AGGREGATOR_ITERATOR_H_

#include <deque>
#include <vector>

#include "dove_eye/frameset.h"


namespace dove_eye {

class Aggregator;

/*
 * Iteration state is kept in iterator, however
 * during its increment underlying video providers are modified thus
 * potentially invalidating any other iterators to the same
 * FramesetAggregator.
 */
class AggregatorIterator {
 public:
  explicit AggregatorIterator(Aggregator *aggregator = nullptr,
                              const bool valid = true);

  explicit AggregatorIterator(const CameraIndex arity);

  AggregatorIterator &operator++();

  inline Frameset operator*() const {
    return frameset_;
  }

  inline bool operator==(const AggregatorIterator &rhs) const {
    return !valid_ && !rhs.valid_;
  }

  inline bool operator!=(const AggregatorIterator &rhs) const {
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

  bool PrepareFrameset();
}; // end class AggregatorIterator


} // namespace dove_eye

#endif // DOVE_EYE_AGGREGATOR_ITERATOR_H_

