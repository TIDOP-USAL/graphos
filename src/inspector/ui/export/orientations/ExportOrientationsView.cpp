#include "ExportOrientationsView.h"

#include <QFileDialog>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QStandardPaths>

namespace inspector
{

namespace ui
{

ExportOrientationsViewImp::ExportOrientationsViewImp(QWidget *parent)
  : ExportOrientationsView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

ExportOrientationsViewImp::~ExportOrientationsViewImp()
{

}

void ExportOrientationsViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportOrientationsView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(400, 200);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mLabelFormat = new QLabel(this);
  mLabelFormat->setMaximumSize(QSize(71, 16777215));
  gridLayout->addWidget(mLabelFormat, 0, 0, 1, 1);

  mComboBoxFormat = new QComboBox(this);
  gridLayout->addWidget(mComboBoxFormat, 0, 1, 1, 1);

  QWidget *widgetFeatureExtractor = new QWidget();
  mGridLayoutFormat = new QGridLayout(widgetFeatureExtractor);
  mGridLayoutFormat->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(widgetFeatureExtractor, 1, 0, 1, 2);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 2, 0, 1, 2);

  this->retranslate();
  this->update();
}

void ExportOrientationsViewImp::initSignalAndSlots()
{
  connect(mComboBoxFormat,         &QComboBox::currentTextChanged,     this, &ExportOrientationsView::formatChange);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &IDialogView::help);
}

void ExportOrientationsViewImp::clear()
{
  const QSignalBlocker blockerComboBoxFormat(mComboBoxFormat);
  mComboBoxFormat->clear();
  update();
}

void ExportOrientationsViewImp::update()
{
//  bool bSave = !mLineEditExportPath->text().isEmpty();
//  mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(bSave);
}

void ExportOrientationsViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("ExportOrientationsView", "Export Cameras", nullptr));
  mLabelFormat->setText(QApplication::translate("ExportOrientationsView", "Format:", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ExportOrientationsView", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("ExportOrientationsView", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ExportOrientationsView", "Help", nullptr));
}

void ExportOrientationsViewImp::addFormatWidget(QWidget *formatWidget)
{
  mComboBoxFormat->addItem(formatWidget->windowTitle());
  mGridLayoutFormat->addWidget(formatWidget, 0, 0, 1, 2);
  formatWidget->setVisible(false);

  this->update();
}

QString ExportOrientationsViewImp::format() const
{
  return mComboBoxFormat->currentText();
}

void ExportOrientationsViewImp::setCurrentFormat(const QString &format)
{
  const QSignalBlocker blockerComboBoxFormat(mComboBoxFormat);
  mComboBoxFormat->setCurrentText(format);

  for (int idx = 0; idx < mGridLayoutFormat->count(); idx++){
    QLayoutItem *item = mGridLayoutFormat->itemAt(idx);
    if(dynamic_cast<QWidgetItem *>(item)){
      if (item->widget()->windowTitle().compare(format) == 0)
        item->widget()->setVisible(true);
      else
        item->widget()->setVisible(false);
    }
  }
}

} // namespace ui

} // namespace inspector
