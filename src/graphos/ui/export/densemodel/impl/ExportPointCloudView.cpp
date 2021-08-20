#include "ExportPointCloudView.h"

#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>

namespace graphos
{

namespace ui
{

ExportPointCloudViewImp::ExportPointCloudViewImp(QWidget *parent)
  : ExportPointCloudView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

ExportPointCloudViewImp::~ExportPointCloudViewImp()
{

}

void ExportPointCloudViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportPointCloudView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(300, 200);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

//  mLabelFormat = new QLabel(this);
//  mLabelFormat->setMaximumSize(QSize(71, 16777215));
//  gridLayout->addWidget(mLabelFormat, 0, 0, 1, 1);

//  mComboBoxFormat = new QComboBox(this);
//  gridLayout->addWidget(mComboBoxFormat, 0, 1, 1, 1);

//  QWidget *widgetPointCloudFormat = new QWidget();
//  mGridLayoutFormat = new QGridLayout(widgetPointCloudFormat);
//  mGridLayoutFormat->setContentsMargins(0, 0, 0, 0);
//  gridLayout->addWidget(widgetPointCloudFormat, 1, 0, 1, 2);

  mCheckBoxColor = new QCheckBox(this);
  gridLayout->addWidget(mCheckBoxColor, 0, 0, 1, 2);

  mCheckBoxNormals = new QCheckBox(this);
  gridLayout->addWidget(mCheckBoxNormals, 1, 0, 1, 2);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 4, 2, 1, 1);

  this->retranslate();
  this->clear();
  this->update();
}

void ExportPointCloudViewImp::initSignalAndSlots()
{
  //connect(mComboBoxFormat,    &QComboBox::currentTextChanged,    this, &ExportPointCloudView::formatChange);
  connect(mCheckBoxColor,     &QCheckBox::clicked,               this, &ExportPointCloudView::colorChange);
  connect(mCheckBoxNormals,   &QCheckBox::clicked,               this, &ExportPointCloudView::normalsChange);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &DialogView::help);
}

void ExportPointCloudViewImp::clear()
{
//  const QSignalBlocker blockerComboBoxFormat(mComboBoxFormat);
//  mComboBoxFormat->clear();

  update();
}

void ExportPointCloudViewImp::update()
{

}

void ExportPointCloudViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("ExportPointCloudViewImp", "Export Point Cloud", nullptr));
  //mLabelFormat->setText(QApplication::translate("ExportPointCloudViewImp", "Format:", nullptr));
  mCheckBoxColor->setText(QApplication::translate("ExportPointCloudViewImp", "Export Color", nullptr));
  mCheckBoxNormals->setText(QApplication::translate("ExportPointCloudViewImp", "Export Normals", nullptr));
}

//void ExportPointCloudViewImp::addFormatWidget(QWidget *formatWidget)
//{
//  mComboBoxFormat->addItem(formatWidget->windowTitle());
//  mGridLayoutFormat->addWidget(formatWidget, 0, 0, 1, 2);
//  formatWidget->setVisible(false);
//
//  this->update();
//}

//QString ExportPointCloudViewImp::format() const
//{
//  return mComboBoxFormat->currentText();
//}

bool ExportPointCloudViewImp::isColorActive() const
{
  return mCheckBoxNormals->isChecked();
}

bool ExportPointCloudViewImp::isNormalsActive() const
{
  return mCheckBoxNormals->isChecked();
}

//void ExportPointCloudViewImp::setCurrentFormat(const QString &format)
//{
//  const QSignalBlocker blockerComboBoxFormat(mComboBoxFormat);
//  mComboBoxFormat->setCurrentText(format);

//  for (int idx = 0; idx < mGridLayoutFormat->count(); idx++){
//    QLayoutItem *item = mGridLayoutFormat->itemAt(idx);
//    if(dynamic_cast<QWidgetItem *>(item)){
//      if (item->widget()->windowTitle().compare(format) == 0)
//        item->widget()->setVisible(true);
//      else
//        item->widget()->setVisible(false);
//    }
//  }
//}

void ExportPointCloudViewImp::setActiveColor(bool active)
{
  mCheckBoxColor->setChecked(true);
}

void ExportPointCloudViewImp::setActiveNormals(bool active)
{
  mCheckBoxNormals->setChecked(true);
}

} // namespace ui

} // namespace graphos


