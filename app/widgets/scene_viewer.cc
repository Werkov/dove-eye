#include "widgets/scene_viewer.h"

#include <cassert>

#include <opencv2/opencv.hpp>

#include "dove_eye/types.h"

using dove_eye::CameraIndex;
using dove_eye::Location;
using qglviewer::Camera;
using qglviewer::Quaternion;
using qglviewer::Vec;

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

void SceneViewer::SetCalibrationData(const dove_eye::CalibrationData &data) {
  CreateCameras(data);
  // TODO something else?
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

  /* Draw cameras */
  glDisable(GL_LIGHTING);
  glLineWidth(4.0);
  glColor4f(1.0, 1.0, 1.0, 0.5);
  for (auto &camera : cameras_) {
    camera->draw();
  }
  glEnable(GL_LIGHTING);
}


void SceneViewer::CreateCameras(const dove_eye::CalibrationData &data) {
  cameras_.resize(data.Arity());

  for (CameraIndex cam = 0; cam < data.Arity(); ++cam) {
    auto camera = new Camera();
    cameras_[cam] = std::move(CameraPtr(camera));
  }

  /*
   * First camera is at the origin, orientation being identity matrix.
   *
   * Obtain other camera position relative to the first camera,
   * utilizing camera pairs indexing.
   */
  for (CameraIndex cam = 1; cam < data.Arity(); ++cam) {
    auto &pair = data.pair_result(cam - 1);
    auto &T = pair.translation;
    auto &R = pair.rotation;
    assert(T.channels() == 1);
    assert(T.depth() == CV_64F);

    assert(R.channels() == 1);
    assert(R.depth() == CV_64F);

    const cv::Vec3d t(T);
    cameras_[cam]->setPosition(Vec(t));
   
    const cv::Vec3d c0(R.col(0));
    const cv::Vec3d c1(R.col(1));
    const cv::Vec3d c2(R.col(2));
    
    Quaternion q;
    q.setFromRotatedBasis(Vec(c0), Vec(c1), Vec(c2));
    cameras_[cam]->setOrientation(q);
  }

}

} // end namespace widgets
