#include "widgets/camera_video_provider.h"

#include "ui_camera_video_provider.h"

#include <QSize>
#include <QString>

namespace widgets {

CameraVideoProvider::CameraVideoProvider(dove_eye::CameraVideoProvider *provider, QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::CameraVideoProvider),
      provider_(provider) {
  ui_->setupUi(this);
  InitializeUi();

  setFocusProxy(ui_->chk_enabled);
}

CameraVideoProvider::~CameraVideoProvider() {
}

bool CameraVideoProvider::Selected() const {
  return ui_->chk_enabled->isChecked();
}

void CameraVideoProvider::ApplySettings() {
  auto selected_value = ui_->cmb_resolution->currentData();

  if (selected_value.isValid()) {
    QSize resolution(selected_value.toSize());
    provider()->resolution(resolution.width(), resolution.height());
  } else {
    provider()->SetDefaultResolution();
  }
}

void CameraVideoProvider::InitializeUi() {
  ui_->chk_enabled->setText(provider()->Id().c_str());

  ui_->cmb_resolution->clear();
  ui_->cmb_resolution->addItem("Default");
  for (auto resolution : provider()->AvailableResolutions()) {
    QString label = QString("%0 x %1")
        .arg(resolution.width)
        .arg(resolution.height);

    QSize data(resolution.width, resolution.height);

    ui_->cmb_resolution->addItem(label, data);
  }
}
} // end namespace widgets
