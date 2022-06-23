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

#include "TabView.h"

namespace graphos
{
	
TabViewImp::TabViewImp(QWidget *parent)
  : TabView(parent)
{
  initUI();
  initSignalAndSlots();
}

void TabViewImp::initUI()
{
  this->setObjectName(QString("TabHandler"));
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  this->setTabsClosable(true);

  //this->initActions();
  this->retranslate();
  this->update();
}

void TabViewImp::initSignalAndSlots()
{
  /// Mover a presentador
  //connect(this, SIGNAL(tabCloseRequested(int)),                     this, SLOT(hideTab(int)));
  //connect(this, SIGNAL(currentChanged(int)),                        this, SLOT(onTabChanged(int)));
  //connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTabWidgetContextMenu(const QPoint &)));
}

void TabViewImp::clear()
{

}

void TabViewImp::update()
{

}

void TabViewImp::retranslate()
{
}

void TabViewImp::closeTab(int tabId)
{
}

void TabViewImp::setCurrentTab(int tabId)
{
}

bool TabViewImp::imageLoaded(size_t imageId) const
{
  for (int i = 0; i < this->count(); i++) {
 
  }

  return false;
}

void TabViewImp::changeEvent(QEvent *event)
{
}


} // namespace graphos
