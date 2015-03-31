#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <memory>

#include <QBasicTimer>
#include <QObject>
#include <QPoint>

#include "dove_eye/async_policy.h"
#include "dove_eye/camera_calibration.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/localization.h"
#include "dove_eye/parameters.h"
#include "dove_eye/tracker.h"
#include "dove_eye/types.h"
#include "gui/gui_mark.h"


/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class Controller : public QObject {
  Q_OBJECT

 public:
  typedef dove_eye::FramesetAggregator<dove_eye::AsyncPolicy<true>>
      Aggregator;

  enum Mode {
    kIdle,
    kCalibration,
    kTracking
  };

  /**
   * @note Controller takes ownership of all ctor arguments given by pointer
   */
  Controller(dove_eye::Parameters &parameters,
                      Aggregator *aggregator,
                      dove_eye::CameraCalibration *calibration,
                      dove_eye::Tracker *tracker,
                      dove_eye::Localization *localization)
      : QObject(),
        parameters_(parameters),
        mode_(kIdle),
        arity_(aggregator->Arity()),
        frameset_iterator_(aggregator->Arity()),
        frameset_end_iterator_(aggregator->Arity()),
        aggregator_(aggregator),
        calibration_(calibration),
        tracker_(tracker),
        localization_(localization) {
  }

  inline dove_eye::CameraIndex Arity() const {
    return aggregator_->Arity();
  }

  inline Mode mode() const {
    return mode_;
  }

 signals:
  void FramesetReady(const dove_eye::Frameset &);
  void PositsetReady(const dove_eye::Positset &);
  void LocationReady(const dove_eye::Location &);
  void ModeChanged(const Controller::Mode new_mode);

  void CameraCalibrationProgressed(const dove_eye::CameraIndex cam,
                                   const double progress);
  void PairCalibrationProgressed(const dove_eye::CameraIndex index,
                                 const double progress);

 public slots:
  // TODO remove start/stop?
  void Start();

  void Stop();

  void SetMark(const dove_eye::CameraIndex cam, const gui::GuiMark mark);

  void SetMode(const Mode mode);


 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  const dove_eye::Parameters &parameters_;
  Mode mode_;
  const dove_eye::CameraIndex arity_;

  QBasicTimer timer_;
  Aggregator::Iterator frameset_iterator_;
  Aggregator::Iterator frameset_end_iterator_;

  std::unique_ptr<Aggregator> aggregator_;
  std::unique_ptr<dove_eye::CameraCalibration> calibration_;
  std::unique_ptr<dove_eye::Tracker> tracker_;
  std::unique_ptr<dove_eye::Localization> localization_;

  void DecorateFrameset(dove_eye::Frameset &frameset,
                        const dove_eye::Positset positset);
};

Q_DECLARE_METATYPE(Controller::Mode)

#endif // CONTROLLER_H_
