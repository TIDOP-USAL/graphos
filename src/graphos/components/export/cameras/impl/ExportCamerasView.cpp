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

#include "ExportCamerasView.h"

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

namespace graphos
{

ExportCamerasViewImp::ExportCamerasViewImp(QWidget *parent)
  : ExportCamerasView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

ExportCamerasViewImp::~ExportCamerasViewImp()
{

}

void ExportCamerasViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportCamerasView"));
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

void ExportCamerasViewImp::initSignalAndSlots()
{
  connect(mComboBoxFormat,         &QComboBox::currentTextChanged,     this, &ExportCamerasView::formatChange);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &ExportCamerasView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &DialogView::help);
}

void ExportCamerasViewImp::clear()
{
  const QSignalBlocker blockerComboBoxFormat(mComboBoxFormat);
  mComboBoxFormat->clear();
  update();
}

void ExportCamerasViewImp::update()
{
}

void ExportCamerasViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("ExportCamerasView", "Export Cameras", nullptr));
  mLabelFormat->setText(QApplication::translate("ExportCamerasView", "Format:", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ExportCamerasView", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("ExportCamerasView", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ExportCamerasView", "Help", nullptr));
}

void ExportCamerasViewImp::addFormatWidget(QWidget *formatWidget)
{
  mComboBoxFormat->addItem(formatWidget->windowTitle());
  mGridLayoutFormat->addWidget(formatWidget, 0, 0, 1, 2);
  formatWidget->setVisible(false);

  this->update();
}

QString ExportCamerasViewImp::format() const
{
  return mComboBoxFormat->currentText();
}

void ExportCamerasViewImp::setCurrentFormat(const QString &format)
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

} // namespace graphos
