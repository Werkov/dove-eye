#include "controller.h"

#include <cassert>

#include <opencv2/opencv.hpp>
#include <QTimerEvent>

#include "dove_eye/inner_tracker.h"
#include "dove_eye/location.h"

using dove_eye::CalibrationData;
using dove_eye::CameraIndex;
using dove_eye::Frameset;
using dove_eye::InnerTracker;
using dove_eye::Location;
using dove_eye::Parameters;
using gui::GuiMark;
using std::unique_ptr;


/** Start main controller loop
 *
 * There's currently no stop method, controller is just destroyed
 */
void Controller::Start(bool paused) {
  frameset_iterator_ = aggregator_->begin();
  frameset_end_iterator_ = aggregator_->end();

  SetMode(kIdle);
  if (paused) {
    emit Paused();
  } else {
    timer_.start(0, this);
    emit Started();
  }
}

void Controller::Stop() {
  timer_.stop();
  emit Finished();
}

void Controller::Pause() {
  timer_.stop();
  emit Paused();
}

void Controller::Step() {
  if (!FramesetLoop()) {
    emit Finished();
  }
}

void Controller::Resume() {
  // TODO is check that aggregator didn't finish necessary ?
  timer_.start(0, this);
  emit Started();
}

void Controller::SetMark(const dove_eye::CameraIndex cam,
                         const GuiMark gui_mark) {
  if (!calibration_data_) {
    return;
  }

  auto mark = GuiMarkToMark(gui_mark);

  bool project_other = false;
  if (gui_mark.flags & GuiMark::kCtrl) {
    project_other = true;
  }

  if (tracker_->SetMark(cam, mark, project_other)) {
    SetMode(kTracking);
  }
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

void Controller::SetUndistortMode(const UndistortMode undistort_mode) {
  undistort_mode_ = undistort_mode;

  switch (undistort_mode_) {
    case kIgnoreDistortion:
      tracker_->distorted_input(false);
      UndistortToProviders(false);
      break;

    case kUndistortVideo:
      tracker_->distorted_input(false);
      UndistortToProviders(true);

      break;
    case kUndistortData:
      tracker_->distorted_input(true);
      UndistortToProviders(false);
      break;
  }
}

void Controller::SetTrackerMarkType(const TrackerMarkType mark_type) {
  tracker_mark_type_ = mark_type;
}

void Controller::SetLocalizationActive(const bool value) {
  localization_active_ = value;
}

void Controller::SetCalibrationData(const CalibrationData calibration_data) {
  /* Before we delete old calibration_data update references. */
  auto new_calibration_data = new CalibrationData(calibration_data);

  assert(tracker_);
  tracker_->calibration_data(new_calibration_data);

  assert(localization_);
  localization_->calibration_data(new_calibration_data);

  CalibrationDataToProviders(new_calibration_data);

  calibration_data_.reset(new_calibration_data);
}

void Controller::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  if (!FramesetLoop()) {
    timer_.stop();
    emit Finished();
  }
}

/** Main capture-track-localize loop
 */
bool Controller::FramesetLoop() {
  if (frameset_iterator_ == frameset_end_iterator_) {
    return false;
  }

  auto frameset = *frameset_iterator_;
  dove_eye::Positset positset(Arity());

  switch (mode_) {
    case kIdle:
      break;
    case kCalibration:
      if (calibration_->MeasureFrameset(frameset)) {
        SetMode(kIdle);
        /*
         * This will notify the application and it will signal back to us,
         * to update tracker, etc.
         */
        emit CalibrationDataReady(calibration_->Data());
        break;
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

      if (localization_active_) {
        Location location;
        if (localization_->Locate(positset, &location)) {
          DEBUG("loc: %f %f %f", location.x, location.y, location.z);
          emit LocationReady(location);
        }
      }
      break;
    }
    case kNonexistent:
      assert(false);
      break;
  }


  DecorateFrameset(frameset, positset);

  ++frameset_iterator_;
  return true;
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
                 cv::Scalar(0, 0, 255),
                 3);
    }
  }

  emit FramesetReady(*frameset_iterator_);
}

void Controller::CalibrationDataToProviders(
    const CalibrationData *calibration_data) {

  CameraIndex cam = 0;
  for (auto provider : aggregator_->providers()) {
    provider->camera_parameters(&calibration_data->camera_parameters(cam));
    ++cam;
  }
}

void Controller::UndistortToProviders(const bool undistort) {
  for (auto provider : aggregator_->providers()) {
    provider->undistort(undistort);
  }
}

InnerTracker::Mark Controller::GuiMarkToMark(const GuiMark &gui_mark) const {
  switch (tracker_mark_type_) {
    case kCircle: {
      InnerTracker::Mark mark(InnerTracker::Mark::kCircle);
      if (gui_mark.Size().isEmpty()) {
        mark.center.x = gui_mark.release_pos.x();
        mark.center.y = gui_mark.release_pos.y();
        mark.radius = parameters_.Get(Parameters::TEMPLATE_RADIUS);
      } else {
        /* Inscribed circle */
        mark.center.x = gui_mark.TopLeft().x() + gui_mark.Size().width() / 2;
        mark.center.y = gui_mark.TopLeft().y() + gui_mark.Size().height() / 2;
        mark.radius = std::min(gui_mark.Size().width(), gui_mark.Size().height());
      }
      
      return mark;
    }

    case kRectangle: {
      InnerTracker::Mark mark(InnerTracker::Mark::kRectangle);
      mark.top_left.x = gui_mark.TopLeft().x();
      mark.top_left.y = gui_mark.TopLeft().y();
      mark.size.x = gui_mark.Size().width();
      mark.size.y = gui_mark.Size().height();

      return mark;
    }

    default:
      assert(false);
  }
}


