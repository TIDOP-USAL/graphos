/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "OriTxtFormatWidget.h"

TL_SUPPRESS_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QComboBox>
TL_DEFAULT_WARNINGS

namespace graphos
{


OriTxtFormatWidget::OriTxtFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
}




OriTxtFormatWidgetImp::OriTxtFormatWidgetImp(QWidget *parent)
  : OriTxtFormatWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelRotation(new QLabel(this)),
    mComboBoxRotation(new QComboBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
  OriTxtFormatWidgetImp::initUI();
  OriTxtFormatWidgetImp::initSignalAndSlots();
}

void OriTxtFormatWidgetImp::initUI()
{
  this->setWindowTitle("Ori Format");
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

  OriTxtFormatWidgetImp::retranslate();
  OriTxtFormatWidgetImp::clear(); /// set default values
  OriTxtFormatWidgetImp::update();
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
  mLabelFile->setText(QApplication::translate("OriTxtFormatWidget", "File", nullptr));
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


} // namespace graphos
