#include "OriTxtFormatWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QComboBox>

namespace inspector
{

OriTxtFormatWidgetImp::OriTxtFormatWidgetImp(QWidget *parent)
  : OriTxtFormatWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelRotation(new QLabel(this)),
    mComboBoxRotation(new QComboBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

void OriTxtFormatWidgetImp::initUI()
{
  this->setWindowTitle("TXT");
  this->setObjectName("OriTxtFormatWidget");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelRotation, 0, 0, 1, 1);
  mComboBoxRotation->addItem("Omega Phi Kappa");
  mComboBoxRotation->addItem("Quaternions");
  propertiesLayout->addWidget(mComboBoxRotation, 0, 1, 1, 1);

  propertiesLayout->addWidget(mLabelFile, 1, 0, 1, 1);
  propertiesLayout->addWidget(mLineEditFile, 1, 1, 1, 1);
  mPushButtonSelectPath->setMaximumSize(QSize(23, 16777215));
  mPushButtonSelectPath->setText("...");
  propertiesLayout->addWidget(mPushButtonSelectPath, 1, 2, 1, 1);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void OriTxtFormatWidgetImp::initSignalAndSlots()
{
  connect(mComboBoxRotation,     &QComboBox::currentTextChanged,   this, &OriTxtFormatWidgetImp::rotationChanged);
  connect(mLineEditFile,         &QLineEdit::textChanged,   this, &OriTxtFormatWidgetImp::fileChanged);
  connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &OriTxtFormatWidgetImp::onPushButtonSelectPath);
}

void OriTxtFormatWidgetImp::clear()
{
  const QSignalBlocker blocker(mLineEditFile);
  const QSignalBlocker blocker2(mComboBoxRotation);
  
  mComboBoxRotation->setCurrentText("Omega Phi Kappa");
  mLineEditFile->clear();
}

void OriTxtFormatWidgetImp::update()
{
}

void OriTxtFormatWidgetImp::retranslate()
{
  mLabelFile->setText(QApplication::translate("OriTxtFormatWidget", "OriTxt File", nullptr));
}

void OriTxtFormatWidgetImp::setFile(const QString &file)
{
  const QSignalBlocker blocker(mLineEditFile);
  mLineEditFile->setText(file);
}

QString OriTxtFormatWidgetImp::file() const
{
  return mLineEditFile->text();
}

QString OriTxtFormatWidgetImp::rotation() const
{
  return mComboBoxRotation->currentText();
}

void OriTxtFormatWidgetImp::setRotation(const QString &rotation)
{
  const QSignalBlocker blocker(mComboBoxRotation);
  mComboBoxRotation->setCurrentText(rotation);
}

void OriTxtFormatWidgetImp::onPushButtonSelectPath()
{
  QString pathName = QFileDialog::getSaveFileName(nullptr,
      tr("Txt file"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
      tr("Orientation plain text (*.txt)"));

  if (!pathName.isEmpty()) {
    mLineEditFile->setText(pathName);
  }

  update();
}

} // namespace inspector
