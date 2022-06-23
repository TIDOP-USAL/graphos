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

#ifndef GRAPHOS_TAB_VIEW_H
#define GRAPHOS_TAB_VIEW_H

#include "graphos/components/tab/TabView.h"

namespace graphos
{

class TabViewImp
  : public TabView
{
  Q_OBJECT

public:

  explicit TabViewImp(QWidget *parent = nullptr);
  ~TabViewImp() override = default;

private:

  void initUI();
  //void initActions();
  void initSignalAndSlots();
  void retranslate();
  void clear();
  void update();

// TabView interface

public:

  void closeTab(int tabId) override;
  void setCurrentTab(int tabId) override;
  bool imageLoaded(size_t imageId) const override;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override;

protected:

};

} // namespace graphos


#endif // GRAPHOS_TAB_VIEW_H
