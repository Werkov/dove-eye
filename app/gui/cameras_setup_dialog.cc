#include "gui/cameras_setup_dialog.h"

#include <QLabel>
#include <QPushButton>

#include "ui_cameras_setup_dialog.h"
#include "widgets/video_provider.h"

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
  auto layout = new QVBoxLayout();

  QWidget *first_widget = nullptr;
  for (auto provider : providers) {
    auto widget = new widgets::VideoProvider(provider);
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

  for (auto video_provider : this->findChildren<widgets::VideoProvider *>()) {
    if (video_provider->Selected()) {
      result.push_back(video_provider->provider());
    }
  }

  emit SelectedProviders(result);
}

} // end namespace gui
