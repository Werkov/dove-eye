#ifndef GUI_VIDEO_PROVIDERS_DIALOG_H_
#define GUI_VIDEO_PROVIDERS_DIALOG_H_

#include <memory>

#include <QDialog>

#include "application.h"

namespace Ui {
class VideoProvidersDialog;
}

namespace gui {

class VideoProvidersDialog : public QDialog {
  Q_OBJECT
 public:
  explicit VideoProvidersDialog(QWidget *parent = nullptr);

  ~VideoProvidersDialog() override;

 signals:
  void SelectedProviders(const Application::VideoProvidersVector &providers) const;

 public slots:
  void SetProviders(const Application::VideoProvidersVector &providers);

 private slots:
  void OnAccepted() const;

 private:
  std::unique_ptr<Ui::VideoProvidersDialog> ui_;

};

} // end namespace gui

#endif // GUI_VIDEO_PROVIDERS_DIALOG_H_
