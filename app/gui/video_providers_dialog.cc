#include "gui/video_providers_dialog.h"

#include <QLabel>

#include "ui_video_providers_dialog.h"
#include "widgets/video_provider.h"

namespace gui {

VideoProvidersDialog::VideoProvidersDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::VideoProvidersDialog) {
  connect(this, &QDialog::accepted,
          this, &VideoProvidersDialog::OnAccepted);
  ui_->setupUi(this);
}

VideoProvidersDialog::~VideoProvidersDialog() {
}

void VideoProvidersDialog::SetProviders(
    const Application::VideoProvidersVector &providers) {
  auto layout = new QVBoxLayout();

  for (auto provider : providers) {
    layout->addWidget(new widgets::VideoProvider(provider));
  }

  if (providers.size() == 0) {
    auto empty_message = new QLabel("No cameras found.");
    layout->addWidget(empty_message);
  }

  delete ui_->frame->layout();
  ui_->frame->setLayout(layout);
}

void VideoProvidersDialog::OnAccepted() const {
  Application::VideoProvidersVector result;

  for (auto video_provider : this->findChildren<widgets::VideoProvider *>()) {
    if (video_provider->Selected()) {
      result.push_back(video_provider->provider());
    }
  }

  emit SelectedProviders(result);
}

} // end namespace gui
