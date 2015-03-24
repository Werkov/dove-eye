#include "controller.h"

#include <QTimerEvent>

using dove_eye::Frameset;

namespace gui {

void Controller::Start() {
  frameset_iterator_ = provider_.begin();
  frameset_end_iterator_ = provider_.end();

  timer_.start(0, this);
}

void Controller::Stop() {
  timer_.stop();
}

void Controller::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  if (frameset_iterator_ == frameset_end_iterator_) {
    timer_.stop();
    return;
  }

  auto frameset = *frameset_iterator_;
  emit FramesetReady(*frameset_iterator_);

  auto positset = tracker_.Track(frameset);
  emit PositsetReady(positset);

  auto location = localization_.Locate(positset);
  emit LocationReady(location);

  ++frameset_iterator_;
}

} // namespace gui
