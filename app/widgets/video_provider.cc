#include "widgets/video_provider.h"

#include "ui_video_provider.h"

namespace widgets {

VideoProvider::VideoProvider(dove_eye::VideoProvider *provider, QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::VideoProvider),
      provider_(provider) {
  ui_->setupUi(this);
  ui_->chk_enabled->setText(provider->Id().c_str());
}

VideoProvider::~VideoProvider() {
}

bool VideoProvider::Selected() const {
  return ui_->chk_enabled->isChecked();
}

} // end namespace widgets
