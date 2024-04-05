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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_FLOOR_LEVEL_COMPONENT_H
#define GRAPHOS_FLOOR_LEVEL_COMPONENT_H

#include "graphos/core/Component.h"

class ccPointCloud;

namespace graphos
{

class FloorLevelComponent
  : public Component
{

    Q_OBJECT

public:

    FloorLevelComponent();
    ~FloorLevelComponent() override;

public slots:

    void open();

private slots:

    void pointClicked(const QVector3D &point);

private:

    void init();
    void update();

signals:

    void open_3d_model();

// Component interface

public:

    auto name() const -> QString override;
    auto action() const -> QAction* override;
    auto menu() const -> QString override;
    auto toolbar() const -> QString override;
    auto widget() const -> QWidget* override;
    auto command() -> std::shared_ptr<Command> override;
    void setName(const QString &name) override;
    void setMenu(const QString &menu) override;
    void setToolbar(const QString &toolbar) override;
    void setIcon(const QIcon &icon) override;
    void freeMemory() override;

private:

    QString mMenu;
    QString mToolbar;
    QString mName;
    QAction *mAction;
    Model *mModel;
    View *mView;
    Presenter *mPresenter;
    std::shared_ptr<Command> mCommand;
    Application *mApplication;
    bool mDeleteView;
    ccPointCloud *mLevelMarkersCloud;
    std::vector<QVector3D> mPoints;

};

} // namespace graphos


#endif // GRAPHOS_FLOOR_LEVEL_COMPONENT_H
