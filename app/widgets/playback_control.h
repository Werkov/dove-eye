#ifndef WIDGETS_PLAYBACK_CONTROL_H_
#define WIDGETS_PLAYBACK_CONTROL_H_

#include <memory>

#include <QWidget>


namespace Ui {
class PlaybackControl;
}

namespace widgets {

class PlaybackControl : public QWidget {
  Q_OBJECT

  enum State {
    kStopped,
    kPaused,
    kPlaying
  };

 public:
  explicit PlaybackControl(QWidget *parent = nullptr);

  ~PlaybackControl() override;

 signals:
  void Started(bool paused);

  void Paused();

  void Resumed();

  void Stepped();

 public slots:
  void Start();
  void Pause();
  void Finish();

 private slots:
  void PlayClicked();
  void PauseClicked();
  void StepClicked();
  void SetState(State state);

 private:

  std::unique_ptr<Ui::PlaybackControl> ui_;
  State state_;

};

} // end namespace widgets

#endif // WIDGETS_PLAYBACK_CONTROL_H_
