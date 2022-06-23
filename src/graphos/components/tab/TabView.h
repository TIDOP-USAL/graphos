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

#ifndef GRAPHOS_TAB_VIEW_GRAPHOS_H
#define GRAPHOS_TAB_VIEW_GRAPHOS_H

#include "graphos/core/mvp.h"

#include <QTabWidget>

namespace graphos
{

/*!
 * \brief TabView interface
 */
class TabView
  : public QTabWidget
{

  Q_OBJECT

public:

  TabView(QWidget *parent) : QTabWidget(parent) {}
  virtual ~TabView() override = default;

  virtual void closeTab(int tabId) = 0;
  virtual void setCurrentTab(int tabId) = 0;
  virtual bool imageLoaded(size_t imageId) const = 0;

protected slots:



signals:

  //void closeTab(int);
  void currentTabChanged(int);
};

} // namespace graphos


#endif // GRAPHOS_TAB_VIEW_GRAPHOS_H
