/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "PointCloudCSVFormatWidget.h"


#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>

namespace graphos
{

PointCloudCSVFormatWidgetImp::PointCloudCSVFormatWidgetImp(QWidget *parent)
  : PointCloudCSVFormatWidget(parent)
{
  PointCloudCSVFormatWidgetImp::initUI();
  PointCloudCSVFormatWidgetImp::initSignalAndSlots();
}

void PointCloudCSVFormatWidgetImp::initUI()
{
  this->setWindowTitle("Point Cloud CSV Format");
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

  PointCloudCSVFormatWidgetImp::retranslate();
  PointCloudCSVFormatWidgetImp::clear(); /// set default values
  PointCloudCSVFormatWidgetImp::update();
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
  const QSignalBlocker blocker(mRadioButtonSemicolon);

  mRadioButtonSemicolon->setChecked(true);
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

} // namespace graphos


