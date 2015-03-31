#ifndef WIDGETS_CONTROLLER_STATUS_H_
#define WIDGETS_CONTROLLER_STATUS_H_


#include <QLabel>
#include <QVector>
#include <QWidget>

#include "controller.h"
#include "dove_eye/types.h"


namespace widgets {

class ControllerStatus : public QWidget {
  Q_OBJECT
 public:
  explicit ControllerStatus(QWidget *parent = nullptr);

 public slots:
  void ModeChanged(const Controller::Mode mode);

 private:
  QLabel *label_;
};

} // end namespace widgets

#endif // WIDGETS_CONTROLLER_STATUS_H_
