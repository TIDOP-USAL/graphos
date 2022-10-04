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

#include "ProgressBarWidget.h"

#include <QProgressBar>
#include <QAction>
#include <QGridLayout>
#include <QApplication>
#include <QToolBar>

namespace graphos
{


ProgressBarWidget::ProgressBarWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mProgressBar(new QProgressBar(this)),
  	mActionCancel(new QAction(this)),
	  mActionMaximize(new QAction(this))
{
  ProgressBarWidget::initUI();
  ProgressBarWidget::initSignalAndSlots();
}

ProgressBarWidget::~ProgressBarWidget()
{

}

void ProgressBarWidget::setRange(int min, int max)
{
  mProgressBar->setRange(min, max);
}

void ProgressBarWidget::setValue(int value)
{
  mProgressBar->setValue(value);
}

void ProgressBarWidget::onPushButtonCancelClicked()
{
  mActionCancel->setEnabled(false);
  emit cancel();
}

void ProgressBarWidget::initUI()
{
  this->setObjectName(QStringLiteral("ProgressBarWidget"));

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  layout->setMargin(0);
  this->setLayout(layout);

  layout->addWidget(mProgressBar, 0, 0);
  mProgressBar->setValue(0);

  mActionCancel->setIcon(QIcon::fromTheme("cancel"));
  mActionMaximize->setIcon(QIcon::fromTheme("minimize-window"));
 
  QToolBar *toolBar = new QToolBar(this);
  toolBar->addAction(mActionCancel);
  toolBar->addAction(mActionMaximize);
  //toolBar->setFixedHeight(20);
  toolBar->setIconSize(QSize(20, 20));
  layout->addWidget(toolBar, 0, 1);

  ProgressBarWidget::retranslate();
  ProgressBarWidget::clear();
  ProgressBarWidget::update();
}

void ProgressBarWidget::initSignalAndSlots()
{
  connect(mActionCancel,   &QAction::triggered, this, &ProgressBarWidget::onPushButtonCancelClicked);
  connect(mActionMaximize, &QAction::triggered, this, &ProgressBarWidget::maximized);
}

void ProgressBarWidget::clear()
{
}

void ProgressBarWidget::update()
{
}

void ProgressBarWidget::retranslate()
{
  mActionCancel->setText(QApplication::translate("ProgressBarWidget", "Cancel Process"));
  mActionMaximize->setText(QApplication::translate("ProgressBarWidget", "Maximize"));
}



} // namespace photomatch
