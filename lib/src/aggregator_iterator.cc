#include "dove_eye/aggregator_iterator.h"

#include "dove_eye/aggregator.h"

namespace dove_eye {

AggregatorIterator::AggregatorIterator(Aggregator *aggregator, const bool valid)
    : aggregator_(aggregator),
      valid_(valid && aggregator && aggregator->Arity() > 0),
      window_start_(0),
      queues_(aggregator ? aggregator->Arity() : 0),
      frameset_(aggregator ? aggregator->Arity() : 0) {
  /* If it's begin iterator, start the reader */
  if (aggregator_ && valid) {
    aggregator_->Start();
  }
}

AggregatorIterator::AggregatorIterator(const CameraIndex arity)
    : aggregator_(nullptr),
      valid_(false),
      window_start_(0),
      queues_(0),
      frameset_(arity) {
}


AggregatorIterator &AggregatorIterator::operator++() {
  Frame frame;
  CameraIndex cam;
  bool frameset_created = false;

  do {
    if (valid_ && !aggregator_->GetFrame(&frame, &cam)) {
      valid_ = false;
      return *this;
    }

    /*
     * Apply offset,
     * see http://www.ms.mff.cuni.cz/~koutnym/wiki/dove_eye/calibration/time
     */
    frame.timestamp -=
        aggregator_->parameters().Get(Parameters::CAM_OFFSET, cam);

    queues_[cam].push_back(frame);

    auto window_size =
        aggregator_->parameters().Get(Parameters::AGGREGATOR_WINDOW);

    /* Move the window forwards? */
    if (frame.timestamp > window_start_ + window_size) {
      window_start_ = frame.timestamp - window_size;
      frameset_created = PrepareFrameset();
      frameset_.sequence_no += 1;
    }
  } while (!frameset_created);

  return *this;
}

bool AggregatorIterator::PrepareFrameset() {
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

} // namespace dove_eye


