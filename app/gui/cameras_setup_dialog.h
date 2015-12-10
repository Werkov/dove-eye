#ifndef GUI_CAMERAS_SETUP_DIALOG_H_
#define GUI_CAMERAS_SETUP_DIALOG_H_

#include <memory>

#include <QDialog>

#include "application.h"

namespace Ui {
class CamerasSetupDialog;
}

namespace gui {

class CamerasSetupDialog : public QDialog {
  Q_OBJECT
 public:
  explicit CamerasSetupDialog(QWidget *parent = nullptr);

  ~CamerasSetupDialog() override;

 signals:
  void SelectedProviders(const Application::ProvidersType type,
                         const Application::VideoProvidersVector &providers) const;

 public slots:
  void SetProviders(const Application::VideoProvidersVector &providers);

 private slots:
  void OnAccepted() const;

 private:
  std::unique_ptr<Ui::CamerasSetupDialog> ui_;

};

} // end namespace gui

#endif // GUI_CAMERAS_SETUP_DIALOG_H_
