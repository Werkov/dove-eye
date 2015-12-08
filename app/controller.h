#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <memory>

#include <QBasicTimer>
#include <QObject>
#include <QPoint>

#include "dove_eye/blocking_policy.h"
#include "dove_eye/calibration_data.h"
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
  typedef dove_eye::FramesetAggregator<dove_eye::BlockingPolicy>
      Aggregator;

  enum Mode {
    kNonexistent,

    /* Just displaying video streams */
    kIdle,

    /* Using video streams for camera calibration */
    kCalibration,

    /* Tracking the video data from FramesetAggregator */
    kTracking
  };

  enum UndistortMode {
    kIgnoreDistortion,
    kUndistortData,
    kUndistortVideo
  };

  enum TrackerMarkType {
    kCircle,
    kRectangle
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
        undistort_mode_(kIgnoreDistortion),
        tracker_mark_type_(kCircle),
        localization_active_(false),
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

  inline UndistortMode undistort_mode() const {
    return undistort_mode_;
  }

 signals:
  void FramesetReady(const dove_eye::Frameset);
  void PositsetReady(const dove_eye::Positset);
  void LocationReady(const dove_eye::Location);
  void ModeChanged(const Controller::Mode new_mode);

  void CameraCalibrationProgressed(const dove_eye::CameraIndex cam,
                                   const double progress);
  void PairCalibrationProgressed(const dove_eye::CameraIndex index,
                                 const double progress);

  void CalibrationDataReady(const dove_eye::CalibrationData);

  void Started();
  void Paused();
  void Finished();

 public slots:
  void Start(bool paused);
  void Stop();

  void Pause();
  void Step();
  void Resume();

  void SetMark(const dove_eye::CameraIndex cam, const gui::GuiMark mark);

  void SetMode(const Mode mode);

  void SetUndistortMode(const UndistortMode undistort_mode);

  void SetTrackerMarkType(const TrackerMarkType mark_type);

  void SetLocalizationActive(const bool value);

  void SetCalibrationData(const dove_eye::CalibrationData calibration_data);

 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  const dove_eye::Parameters &parameters_;

  Mode mode_;
  UndistortMode undistort_mode_;
  TrackerMarkType tracker_mark_type_;
  bool localization_active_;

  const dove_eye::CameraIndex arity_;

  /** Pointer to calibration data
   * Reasons for pointer over reference:
   *   - it may not be initialized at the beginning
   *   - properly implemented calibration data won't be assignable (arity)
   * IMPORTANT it must be prior any dependants (Aggregator, Tracker, ...)
   *           because of destruction order (mind other threads)
   */
  std::unique_ptr<dove_eye::CalibrationData> calibration_data_;


  QBasicTimer timer_;
  Aggregator::Iterator frameset_iterator_;
  Aggregator::Iterator frameset_end_iterator_;

  std::unique_ptr<Aggregator> aggregator_;
  std::unique_ptr<dove_eye::CameraCalibration> calibration_;
  std::unique_ptr<dove_eye::Tracker> tracker_;
  std::unique_ptr<dove_eye::Localization> localization_;

  bool FramesetLoop();

  void DecorateFrameset(dove_eye::Frameset &frameset,
                        const dove_eye::Positset positset);

  void CalibrationDataToProviders(
      const dove_eye::CalibrationData *calibration_data);

  void UndistortToProviders(const bool undistort);

  dove_eye::InnerTracker::Mark GuiMarkToMark(const gui::GuiMark &gui_mark) const;

};

Q_DECLARE_METATYPE(Controller::Mode)
Q_DECLARE_METATYPE(Controller::UndistortMode)

#endif // CONTROLLER_H_
