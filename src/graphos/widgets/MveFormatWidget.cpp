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


MveFormatWidget::MveFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
}



MveFormatWidgetImp::MveFormatWidgetImp(QWidget *parent)
  : MveFormatWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
  MveFormatWidgetImp::initUI();
  MveFormatWidgetImp::initSignalAndSlots();
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

  MveFormatWidgetImp::retranslate();
  MveFormatWidgetImp::clear(); /// set default values
  MveFormatWidgetImp::update();
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
