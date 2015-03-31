#include "controller.h"

#include <cassert>

#include <opencv2/opencv.hpp>
#include <QTimerEvent>

#include "dove_eye/inner_tracker.h"

using dove_eye::CameraIndex;
using dove_eye::Frameset;
using dove_eye::InnerTracker;
using dove_eye::Parameters;
using gui::GuiMark;


void Controller::Start() {
  frameset_iterator_ = aggregator_->begin();
  frameset_end_iterator_ = aggregator_->end();

  SetMode(kIdle);
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
  tracker_->SetMark(cam, mark, project_other);
}

void Controller::SetMode(const Mode mode) {
  mode_ = mode;

  switch (mode_) {
    case kCalibration:
      calibration_->Reset();
      break;
    default:
      /* empty */
      break;
  }

  emit ModeChanged(mode_);
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
  dove_eye::Positset positset(Arity());

  switch (mode_) {
    case kIdle:
      break;
    case kCalibration:
      if (calibration_->MeasureFrameset(frameset)) {
        SetMode(kIdle);
        // TODO set result to application
      }

      for (CameraIndex cam = 0; cam < Arity(); ++cam) {
        emit CameraCalibrationProgressed(cam,
                                         calibration_->CameraProgress(cam));
      }
      for (auto pair : calibration_->pairs()) {
        emit PairCalibrationProgressed(pair.index,
                                       calibration_->PairProgress(pair.index));
      }

      break;
    case kTracking: {
      positset = tracker_->Track(frameset);
      emit PositsetReady(positset);

      auto location = localization_->Locate(positset);
      emit LocationReady(location);
      break;
    }
    case kNonexistent:
      assert(false);
      break;
  }

  DecorateFrameset(frameset, positset);

  ++frameset_iterator_;
}

void Controller::DecorateFrameset(dove_eye::Frameset &frameset,
                                  const dove_eye::Positset positset) {
  for (CameraIndex cam = 0; cam < frameset.Arity(); ++cam) {
    if (!frameset.IsValid(cam)) {
      continue;
    }

    if (positset.IsValid(cam)) {
      cv::circle(frameset[cam].data, positset[cam],
                 parameters_.Get(Parameters::TEMPLATE_RADIUS),
                 cv::Scalar(0, 255, 0));
    }
  }

  emit FramesetReady(*frameset_iterator_);
}

