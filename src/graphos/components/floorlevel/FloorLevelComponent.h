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
//class cc2DLabel;

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

    virtual QString name() const override;
    virtual QAction *action() const override;
    virtual QString menu() const override;
    virtual QString toolbar() const override;
    virtual QWidget *widget() const override;
    virtual std::shared_ptr<Command> command() override;
    virtual void setName(const QString &name) override;
    virtual void setMenu(const QString &menu) override;
    virtual void setToolbar(const QString &toolbar) override;
    virtual void setIcon(const QIcon &icon) override;
    virtual void freeMemory() override;

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
    //std::vector<cc2DLabel*> s_levelLabels;
    std::vector<QVector3D> mPoints;

};

} // namespace graphos


#endif // GRAPHOS_FLOOR_LEVEL_COMPONENT_H
