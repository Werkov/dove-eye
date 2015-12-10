#include "gui/cameras_setup_dialog.h"

#include <QLabel>
#include <QList>
#include <QPushButton>

#include "dove_eye/camera_video_provider.h"
#include "ui_cameras_setup_dialog.h"
#include "widgets/camera_video_provider.h"

namespace gui {

CamerasSetupDialog::CamerasSetupDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::CamerasSetupDialog) {
  connect(this, &QDialog::accepted,
          this, &CamerasSetupDialog::OnAccepted);
  ui_->setupUi(this);
}

CamerasSetupDialog::~CamerasSetupDialog() {
}

void CamerasSetupDialog::SetProviders(
    const Application::VideoProvidersVector &providers) {
  /*
   * Really? Isn't there better API to delete (direct) children of a widget?
   */
  auto children = ui_->frame->findChildren<QWidget *>();
  QList<QWidget *> to_delete;
  for (auto widget : children) {
    if (widget->parent() == ui_->frame) {
      to_delete.append(widget);
    }
  }
  for (auto widget: to_delete) {
    delete widget;
  }

  /* Now create new widgets for every provider */
  auto layout = new QVBoxLayout();

  QWidget *first_widget = nullptr;
  for (auto provider : providers) {
    auto camera_provider = dynamic_cast<dove_eye::CameraVideoProvider *>(provider);
    assert(camera_provider);
    auto widget = new widgets::CameraVideoProvider(camera_provider);
    if (!first_widget) {
      first_widget = widget;
    }
    layout->addWidget(widget);
  }

  if (providers.size() == 0) {
    auto empty_message = new QLabel("No cameras found.");
    layout->addWidget(empty_message);
  }

  delete ui_->frame->layout();
  ui_->frame->setLayout(layout);
  if (first_widget) {
    first_widget->setFocus();
  }
}

void CamerasSetupDialog::OnAccepted() const {
  Application::VideoProvidersVector result;

  for (auto video_provider : this->findChildren<widgets::CameraVideoProvider *>()) {
    if (video_provider->Selected()) {
      video_provider->ApplySettings();
      result.push_back(video_provider->provider());
    }
  }

  emit SelectedProviders(Application::kCameras, result);
}

} // end namespace gui
