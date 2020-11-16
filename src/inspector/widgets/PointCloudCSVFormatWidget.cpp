#include "PointCloudCSVFormatWidget.h"


#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

namespace inspector
{

PointCloudCSVFormatWidgetImp::PointCloudCSVFormatWidgetImp(QWidget *parent)
  : PointCloudCSVFormatWidget(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

void PointCloudCSVFormatWidgetImp::initUI()
{
  this->setWindowTitle("CSV");
  this->setObjectName("PointCloudCSVFormatWidget");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  mGroupBoxDelimiter = new QGroupBox(this);

  QGridLayout *gridLayoutDelimiter = new QGridLayout(mGroupBoxDelimiter);
  mRadioButtonTab = new QRadioButton(mGroupBoxDelimiter);
  gridLayoutDelimiter->addWidget(mRadioButtonTab, 0, 0, 1, 1);
  mRadioButtonComma = new QRadioButton(mGroupBoxDelimiter);
  gridLayoutDelimiter->addWidget(mRadioButtonComma, 0, 1, 1, 1);
  mRadioButtonSpace = new QRadioButton(mGroupBoxDelimiter);
  gridLayoutDelimiter->addWidget(mRadioButtonSpace, 0, 2, 1, 1);
  mRadioButtonSemicolon = new QRadioButton(mGroupBoxDelimiter);
  gridLayoutDelimiter->addWidget(mRadioButtonSemicolon, 0, 3, 1, 1);
  layout->addWidget(mGroupBoxDelimiter);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void PointCloudCSVFormatWidgetImp::initSignalAndSlots()
{
  connect(mRadioButtonTab, &QRadioButton::released, this, &PointCloudCSVFormatWidgetImp::onDelimiterChanged);
  connect(mRadioButtonComma, &QRadioButton::released, this, &PointCloudCSVFormatWidgetImp::onDelimiterChanged);
  connect(mRadioButtonSpace, &QRadioButton::released, this, &PointCloudCSVFormatWidgetImp::onDelimiterChanged);
  connect(mRadioButtonSemicolon, &QRadioButton::released, this, &PointCloudCSVFormatWidgetImp::onDelimiterChanged);
}

void PointCloudCSVFormatWidgetImp::clear()
{
//  const QSignalBlocker blocker(mLineEditFile);

//  mLineEditFile->clear();
}

void PointCloudCSVFormatWidgetImp::update()
{
}

void PointCloudCSVFormatWidgetImp::retranslate()
{
  mGroupBoxDelimiter->setTitle(QCoreApplication::translate("PointCloudCSVFormatWidget", "Delimiter", nullptr));
  mRadioButtonTab->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Tab", nullptr));
  mRadioButtonComma->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Comma", nullptr));
  mRadioButtonSpace->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Space", nullptr));
  mRadioButtonSemicolon->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Semicolon", nullptr));
}

QString PointCloudCSVFormatWidgetImp::delimiter() const
{
  QString delimiter;
  if (mRadioButtonTab->isChecked()){
    delimiter = "\t";
  } else if (mRadioButtonComma->isChecked()){
    delimiter = ",";
  } else if (mRadioButtonSpace->isChecked()){
    delimiter = " ";
  } else if (mRadioButtonSemicolon->isChecked()){
    delimiter = ";";
  }
  return delimiter;
}

void PointCloudCSVFormatWidgetImp::setDelimiter(const QString &delimiter)
{
  if (delimiter.compare("\t") == 0){
    mRadioButtonTab->setChecked(true);
  } else if (delimiter.compare(",") == 0){
    mRadioButtonComma->setChecked(true);
  } else if (delimiter.compare(" ") == 0){
    mRadioButtonSpace->setChecked(true);
  } else if (delimiter.compare(";") == 0){
    mRadioButtonSemicolon->setChecked(true);
  }
}

void PointCloudCSVFormatWidgetImp::onDelimiterChanged()
{
  emit delimiterChanged(this->delimiter());
}

} // namespace inspector


