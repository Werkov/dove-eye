#ifndef CONTROLLER_H_
#define CONTROLLER_H_

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
      InnerFrameProvider;

  explicit Controller(dove_eye::Parameters &parameters,
                      InnerFrameProvider &provider,
                      dove_eye::Tracker &tracker,
                      dove_eye::Localization &localization,
                      QObject *parent = nullptr)
      : QObject(parent),
        parameters_(parameters),
        frameset_iterator_(provider.Arity()),
        frameset_end_iterator_(provider.Arity()),
        provider_(provider),
        tracker_(tracker),
        localization_(localization) {
  }

  inline dove_eye::CameraIndex Arity() const {
    return provider_.Arity();
  }

 signals:
  void FramesetReady(const dove_eye::Frameset &);
  void PositsetReady(const dove_eye::Positset &);
  void LocationReady(const dove_eye::Location &);

 public slots:
  void Start();

  void Stop();

  void SetMark(const dove_eye::CameraIndex cam, const gui::GuiMark mark);


 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  const dove_eye::Parameters &parameters_;
  QBasicTimer timer_;
  InnerFrameProvider::Iterator frameset_iterator_;
  InnerFrameProvider::Iterator frameset_end_iterator_;

  InnerFrameProvider &provider_;
  dove_eye::Tracker &tracker_;
  dove_eye::Localization &localization_;

  void DecorateFrameset(dove_eye::Frameset &frameset,
                        const dove_eye::Positset positset);
};


#endif // CONTROLLER_H_
