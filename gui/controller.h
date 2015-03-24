#ifndef GUI_CONTROLLER_H_
#define GUI_CONTROLLER_H_

#include <QBasicTimer>
#include <QObject>

#include "dove_eye/async_policy.h"
#include "dove_eye/frameset_aggregator.h"
#include "dove_eye/localization.h"
#include "dove_eye/tracker.h"
#include "dove_eye/types.h"

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class Controller : public QObject {
  Q_OBJECT

 public:
  typedef dove_eye::FramesetAggregator<dove_eye::AsyncPolicy<true>>
      InnerFrameProvider;

  explicit Controller(InnerFrameProvider &provider,
                      dove_eye::Tracker &tracker,
                      dove_eye::Localization &localization,
                      QObject *parent = nullptr)
      : QObject(parent),
        frameset_iterator_(provider.width()),
        frameset_end_iterator_(provider.width()),
        provider_(provider),
        tracker_(tracker),
        localization_(localization) {
  }

  inline dove_eye::CameraIndex width() const {
    return provider_.width();
  }

 signals:
  void FramesetReady(const dove_eye::Frameset &);
  void PositsetReady(const dove_eye::Positset &);
  void LocationReady(const dove_eye::Location &);

 public slots:
  void Start();

  void Stop();


 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  QBasicTimer timer_;
  InnerFrameProvider::Iterator frameset_iterator_;
  InnerFrameProvider::Iterator frameset_end_iterator_;

  InnerFrameProvider &provider_;
  dove_eye::Tracker &tracker_;
  dove_eye::Localization &localization_;
};

} // namespace gui

#endif // GUI_CONTROLLER_H_
