#ifndef WIDGETS_SCENE_VIEWER_H_
#define WIDGETS_SCENE_VIEWER_H_

#include <QGLViewer/qglviewer.h>

#include "dove_eye/location.h"

namespace widgets {

class SceneViewer : public QGLViewer {
  Q_OBJECT
 public:
  explicit SceneViewer(QWidget *parent = nullptr)
      : QGLViewer(parent) {
  }

 public slots:
  void SetLocation(const dove_eye::Location &location);

  void SetDrawTrajectory(const bool value = true);


 protected:
  void init() override;
  void draw() override;

 private:
  bool draw_trajectory_;

  QVector<dove_eye::Location> trajectory_;

  dove_eye::Location location_;
};

} // end namespace widgets

#endif // WIDGETS_SCENE_VIEWER_H_
