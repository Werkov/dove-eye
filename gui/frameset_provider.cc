#include "frameset_provider.h"

#include <QTimerEvent>

using dove_eye::Frameset;

namespace gui {

void FramesetProvider::Start(InnerFrameProvider &provider) {
  frameset_iterator_ = provider.begin();
  frameset_end_iterator_ = provider.end();

  timer_.start(0, this);
}

void FramesetProvider::Stop() {
  timer_.stop();
}

void FramesetProvider::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  if (frameset_iterator_ == frameset_end_iterator_) {
    timer_.stop();
    return;
  }

  emit FramesetReady(*frameset_iterator_);
  ++frameset_iterator_;
}

} // namespace gui
