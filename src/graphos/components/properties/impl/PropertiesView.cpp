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

#include "PropertiesView.h"

#include <QTreeWidget>
#include <QGridLayout>
#include <QApplication>

namespace graphos
{

PropertiesViewImp::PropertiesViewImp(QWidget *parent)
  : PropertiesView(parent)
{
    PropertiesViewImp::initUI();
    PropertiesViewImp::initSignalAndSlots();
}

void PropertiesViewImp::initUI()
{
    this->setObjectName(QString::fromUtf8("PropertiesView"));

    auto layout = new QGridLayout();
    this->setLayout(layout);

    layout->setSpacing(6);
    layout->setContentsMargins(11, 11, 11, 11);
    layout->setContentsMargins(0, 0, 0, 0);

    mTreeWidgetProperties = new QTreeWidget(this);
    auto header_item = new QTreeWidgetItem();
    mTreeWidgetProperties->setHeaderItem(header_item);

    layout->addWidget(mTreeWidgetProperties, 0, 0, 1, 1);

    PropertiesViewImp::retranslate();
    PropertiesViewImp::update();
}

void PropertiesViewImp::initSignalAndSlots()
{
}

void PropertiesViewImp::clear()
{
    mTreeWidgetProperties->clear();
}

void PropertiesViewImp::update()
{

}

void PropertiesViewImp::retranslate()
{
    setWindowTitle(QCoreApplication::translate("PropertiesView", "Image Properties", nullptr));
    QTreeWidgetItem *header_item = mTreeWidgetProperties->headerItem();
    header_item->setText(0, QCoreApplication::translate("PropertiesView", "Name", nullptr));
    header_item->setText(1, QCoreApplication::translate("PropertiesView", "Value", nullptr));
}

void PropertiesViewImp::setProperties(const std::unordered_map<QString, std::list<std::pair<QString, QString>>> &properties)
{
    mTreeWidgetProperties->clear();
    mTreeWidgetProperties->expandAll();

    for (auto &group : properties) {
        auto item = new QTreeWidgetItem();
        item->setText(0, group.first);

        mTreeWidgetProperties->addTopLevelItem(item);

        for (auto &property : group.second) {
            auto item_property = new QTreeWidgetItem();
            item_property->setText(0, property.first);
            item_property->setText(1, property.second);
            item->addChild(item_property);
        }

        item->setExpanded(true);
    }
}

void PropertiesViewImp::setAlternatingRowColors(bool active)
{
    mTreeWidgetProperties->setAlternatingRowColors(active);
}

} // namespace graphos
