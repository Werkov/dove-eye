#include "widgets/playback_control.h"

#include "ui_playback_control.h"


namespace widgets {

PlaybackControl::PlaybackControl(QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::PlaybackControl) {
  ui_->setupUi(this);

  connect(ui_->btn_play, &QPushButton::clicked,
          this, &PlaybackControl::PlayClicked);
  connect(ui_->btn_pause, &QPushButton::clicked,
          this, &PlaybackControl::PauseClicked);
  connect(ui_->btn_step, &QPushButton::clicked,
          this, &PlaybackControl::StepClicked);

  SetState(kStopped);
}

PlaybackControl::~PlaybackControl() {
}

void PlaybackControl::Start() {
  SetState(kPlaying);
}

void PlaybackControl::Pause() {
  SetState(kPaused);
}

void PlaybackControl::Finish() {
  SetState(kStopped);
}

void PlaybackControl::PlayClicked() {
  switch (state_) {
    case kStopped:
      emit Started(false);
      break;
    case kPaused:
      emit Resumed();
      break;
    case kPlaying:
      /* empty */
      break;
  }
}

void PlaybackControl::PauseClicked() {
  if (state_ == kPlaying) {
    emit Paused();
  }
}

void PlaybackControl::StepClicked() {
  emit Stepped();
}

void PlaybackControl::SetState(State state) {
  state_ = state;

  ui_->btn_play->setEnabled(state_ == kStopped || state_ == kPaused);
  ui_->btn_pause->setEnabled(state_ == kPlaying);
  ui_->btn_step->setEnabled(state_ == kStopped || state_ == kPaused);
}

} // end namespace widgets
