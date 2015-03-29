#ifndef GUI_VIDEO_PROVIDERS_DIALOG_H_
#define GUI_VIDEO_PROVIDERS_DIALOG_H_

#include <memory>

#include <QDialog>

namespace Ui {
class VideoProvidersDialog;
}

namespace gui {

class VideoProvidersDialog : public QDialog {
  Q_OBJECT
 public:
  // TODO accept Application object (that would allow setting providers)
  explicit VideoProvidersDialog(QWidget *parent = nullptr);

  ~VideoProvidersDialog() override;

 private:
  std::unique_ptr<Ui::VideoProvidersDialog> ui_;

};

} // end namespace gui

#endif // GUI_VIDEO_PROVIDERS_DIALOG_H_
