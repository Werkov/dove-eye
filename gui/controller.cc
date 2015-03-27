#include "controller.h"

#include <QTimerEvent>
#include <opencv2/opencv.hpp>

#include "dove_eye/inner_tracker.h"

using dove_eye::CameraIndex;
using dove_eye::Frameset;
using dove_eye::InnerTracker;
using dove_eye::Parameters;

namespace gui {

void Controller::Start() {
  frameset_iterator_ = provider_.begin();
  frameset_end_iterator_ = provider_.end();

  timer_.start(0, this);
}

void Controller::Stop() {
  timer_.stop();
}

void Controller::SetMark(const dove_eye::CameraIndex cam,
                         const GuiMark gui_mark) {
  InnerTracker::Mark mark(gui_mark.pos.x(), gui_mark.pos.y());
  // TODO encapsulate project_other into GuiMark
  bool project_other = false;
  tracker_.SetMark(cam, mark, project_other);
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

  auto positset = tracker_.Track(frameset);
  emit PositsetReady(positset);

  DecorateFrameset(frameset, positset);
  emit FramesetReady(*frameset_iterator_);

  auto location = localization_.Locate(positset);
  emit LocationReady(location);

  ++frameset_iterator_;
}

void Controller::DecorateFrameset(dove_eye::Frameset &frameset,
                                  const dove_eye::Positset positset) {

  for (CameraIndex cam = 0; cam < frameset.Size(); ++cam) {
    if (!frameset.IsValid(cam)) {
      continue;
    }

    if (positset.IsValid(cam)) {
      cv::circle(frameset[cam].data, positset[cam],
                 parameters_.Get(Parameters::TEMPLATE_RADIUS),
                 cv::Scalar(0, 255, 0));
    }
  }
}

} // namespace gui
