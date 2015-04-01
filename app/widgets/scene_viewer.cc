#include "widgets/scene_viewer.h"

using dove_eye::Location;

namespace widgets {

void SceneViewer::SetLocation(const dove_eye::Location &location) {
  location_ = location;

  if (draw_trajectory_) {
    // FIXME use some cyclic/limited buffer
    trajectory_.push_back(location_);
  }
}

void SceneViewer::SetDrawTrajectory(const bool value) {
  if (!value) {
    trajectory_.clear();
  }
}

void SceneViewer::init() {
  setGridIsDrawn();
  setAxisIsDrawn();
  startAnimation();
  SetDrawTrajectory();
}

void SceneViewer::draw() {
  if (draw_trajectory_) {
    glBegin(GL_LINE_STRIP);
    glColor3f(0.5, 0.5, 0.5);
    for (auto location : trajectory_) {
      glVertex3f(location.x, location.y, location.z);
    }
    glEnd();
  }

  glPointSize(10.0f);
  glBegin(GL_POINTS);
      glColor3f(1.0, 0.2f, 0);
      glVertex3f(location_.x, location_.y, location_.z);
  glEnd();
}

} // end namespace widgets
