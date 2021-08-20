#include "MveFormatWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

namespace graphos
{

MveFormatWidgetImp::MveFormatWidgetImp(QWidget *parent)
  : MveFormatWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

void MveFormatWidgetImp::initUI()
{
  this->setWindowTitle("MVE");
  this->setObjectName("MveOrientationFormatWidget");

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

void MveFormatWidgetImp::initSignalAndSlots()
{
  connect(mLineEditFile,         &QLineEdit::textChanged,   this, &MveFormatWidgetImp::fileChanged);
  connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &MveFormatWidgetImp::onPushButtonSelectPath);
}

void MveFormatWidgetImp::clear()
{
  const QSignalBlocker blocker(mLineEditFile);

  mLineEditFile->clear();
}

void MveFormatWidgetImp::update()
{
}

void MveFormatWidgetImp::retranslate()
{
  mLabelFile->setText(QApplication::translate("MveOrientationFormatWidget", "MVE export path", nullptr));
}

void MveFormatWidgetImp::setFile(const QString &file)
{
  const QSignalBlocker blocker(mLineEditFile);
  mLineEditFile->setText(file);
}

QString MveFormatWidgetImp::file() const
{
  return mLineEditFile->text();
}

void MveFormatWidgetImp::onPushButtonSelectPath()
{
   QString pathName = QFileDialog::getExistingDirectory(this,
    tr("Export directory"),
    "",
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!pathName.isEmpty()) {
    mLineEditFile->setText(pathName);
  }

  update();
}

} // namespace graphos
