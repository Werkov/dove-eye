#include "widgets/file_selector.h"

#include "ui_file_selector.h"

#include <QFileDialog>

namespace widgets {

FileSelector::FileSelector(const QString &label, QWidget *parent)
    : QWidget(parent),
      ui_(new Ui::FileSelector) {
  ui_->setupUi(this);

  setFocusProxy(ui_->btn_select);

  connect(ui_->btn_clear, &QPushButton::clicked,
          this, &FileSelector::ClearClicked);
  connect(ui_->btn_select, &QPushButton::clicked,
          this, &FileSelector::SelectClicked);

  SetLabel(label);
}

FileSelector::~FileSelector() {
}

bool FileSelector::Selected() const {
  return !ui_->txt_filename->text().isEmpty();
}

QString FileSelector::Filename() const {
  return ui_->txt_filename->text();
}

const QString FileSelector::Label() const {
  return ui_->lbl_label->text();
}

void FileSelector::SetLabel(const QString &label) {
  ui_->lbl_label->setText(label);
}

void FileSelector::ClearClicked() {
  ui_->txt_filename->setText("");
  ui_->btn_clear->hide();
  ui_->btn_select->show();
}

void FileSelector::SelectClicked() {
  auto filename = QFileDialog::getOpenFileName(this, tr("Open file"), "",
                                               tr("All files (*)"));
  ui_->txt_filename->setText(filename);
  if (!filename.isNull()) {
    ui_->btn_select->hide();
    ui_->btn_clear->show();
  }
}

} // end namespace widgets
