#ifndef GUI_FRAMESET_PROVIDER_H_
#define GUI_FRAMESET_PROVIDER_H_

#include <QBasicTimer>
#include <QObject>

#include "dove_eye/frameset.h"
#include "dove_eye/frameset_aggregator.h"

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class FramesetProvider : public QObject {
  Q_OBJECT

 public:
  typedef dove_eye::FramesetAggregator<dove_eye::BlockingPolicy>
      InnerFrameProvider;

  FramesetProvider(QObject *parent = nullptr)
      : QObject(parent) {
  }

 signals:
  void frameset_ready(const dove_eye::Frameset &);

 public slots:
  void start(InnerFrameProvider &provider);

  void stop();


 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  QBasicTimer timer_;
  InnerFrameProvider::Iterator frameset_iterator_;
  InnerFrameProvider::Iterator frameset_end_iterator_;
};

} // namespace gui

#endif // GUI_FRAMESET_PROVIDER_H_
