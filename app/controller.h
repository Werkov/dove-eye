#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <memory>

#include <QBasicTimer>
#include <QObject>
#include <QPoint>

#include "dove_eye/async_policy.h"
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

  /**
   * @note Controller takes ownership of all ctor arguments given by pointer
   */
  explicit Controller(dove_eye::Parameters &parameters,
                      Aggregator *aggregator,
                      dove_eye::Tracker *tracker,
                      dove_eye::Localization *localization)
      : QObject(),
        parameters_(parameters),
        frameset_iterator_(aggregator->Arity()),
        frameset_end_iterator_(aggregator->Arity()),
        aggregator_(aggregator),
        tracker_(tracker),
        localization_(localization) {
  }

  inline dove_eye::CameraIndex Arity() const {
    return aggregator_->Arity();
  }

 signals:
  void FramesetReady(const dove_eye::Frameset &);
  void PositsetReady(const dove_eye::Positset &);
  void LocationReady(const dove_eye::Location &);

 public slots:
  // TODO remove start/stop?
  void Start();

  void Stop();

  void SetMark(const dove_eye::CameraIndex cam, const gui::GuiMark mark);


 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  const dove_eye::Parameters &parameters_;
  QBasicTimer timer_;
  Aggregator::Iterator frameset_iterator_;
  Aggregator::Iterator frameset_end_iterator_;

  std::unique_ptr<Aggregator> aggregator_;
  std::unique_ptr<dove_eye::Tracker> tracker_;
  std::unique_ptr<dove_eye::Localization> localization_;

  void DecorateFrameset(dove_eye::Frameset &frameset,
                        const dove_eye::Positset positset);
};


#endif // CONTROLLER_H_
