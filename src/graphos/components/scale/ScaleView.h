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


#ifndef GRAPHOS_SCALE_VIEW_INTERFACE_H
#define GRAPHOS_SCALE_VIEW_INTERFACE_H


#include "graphos/core/task/TaskView.h"

namespace graphos
{

class ScaleView
  : public TaskView
{

    Q_OBJECT

public:

    ScaleView(QWidget *parent) : TaskView(parent) {}
    ~ScaleView() override = default;

    virtual double distance() const = 0;
    virtual double distanceReal() const = 0;

public slots:

    virtual void setDistance(double distance) = 0;

signals:

    void distanceChanged(double);
    void enableMeasure(bool);
};

} // namespace graphos


#endif // GRAPHOS_SCALE_VIEW_INTERFACE_H
