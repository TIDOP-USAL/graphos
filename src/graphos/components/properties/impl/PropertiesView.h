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

#ifndef GRAPHOS_PROPERTIES_VIEW_H
#define GRAPHOS_PROPERTIES_VIEW_H

#include "graphos/components/properties/PropertiesView.h"

class QTreeWidget;

namespace graphos
{

class PropertiesViewImp
  : public PropertiesView
{
  Q_OBJECT

public:

  explicit PropertiesViewImp(QWidget *parent = nullptr);
  ~PropertiesViewImp() override = default;

// PropertiesView interface

public:

  void setProperties(const std::unordered_map<QString, std::list<std::pair<QString, QString>>> &properties) override;
  void setAlternatingRowColors(bool active) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QTreeWidget *mTreeWidgetProperties;
  
};

} // namespace graphos


#endif // GRAPHOS_PROPERTIES_VIEW_H
