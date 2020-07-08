#include "NvmFormatWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

namespace inspector
{

NvmFormatWidgetImp::NvmFormatWidgetImp(QWidget *parent)
  : NvmFormatWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

void NvmFormatWidgetImp::initUI()
{
  this->setWindowTitle("NVM");
  this->setObjectName("NvmOrientationFormatWidget");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelFile, 0, 0, 1, 1);
  propertiesLayout->addWidget(mLineEditFile, 0, 1, 1, 1);
  mPushButtonSelectPath->setMaximumSize(QSize(23, 16777215));
  mPushButtonSelectPath->setText("...");
  propertiesLayout->addWidget(mPushButtonSelectPath, 0, 2, 1, 1);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void NvmFormatWidgetImp::initSignalAndSlots()
{
  connect(mLineEditFile,         &QLineEdit::textChanged,   this, &NvmFormatWidgetImp::fileChanged);
  connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &NvmFormatWidgetImp::onPushButtonSelectPath);
}

void NvmFormatWidgetImp::clear()
{
  const QSignalBlocker blocker(mLineEditFile);

  mLineEditFile->clear();
}

void NvmFormatWidgetImp::update()
{
}

void NvmFormatWidgetImp::retranslate()
{
  mLabelFile->setText(QApplication::translate("NvmOrientationFormatWidget", "NVM File", nullptr));
}

void NvmFormatWidgetImp::setFile(const QString &file)
{
  const QSignalBlocker blocker(mLineEditFile);
  mLineEditFile->setText(file);
}

QString NvmFormatWidgetImp::file() const
{
  return mLineEditFile->text();
}

void NvmFormatWidgetImp::onPushButtonSelectPath()
{
  QString pathName = QFileDialog::getSaveFileName(nullptr,
      tr("NVM file"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      tr("NVM (*.nvm)"));

  if (!pathName.isEmpty()) {
    mLineEditFile->setText(pathName);
  }

  update();
}

} // namespace inspector
