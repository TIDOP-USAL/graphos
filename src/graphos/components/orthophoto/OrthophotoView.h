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


#ifndef GRAPHOS_ORTHOPHOTO_VIEW_INTERFACE_H
#define GRAPHOS_ORTHOPHOTO_VIEW_INTERFACE_H


#include "graphos/core/task/TaskView.h"

namespace graphos
{

class OrthophotoView
  : public TaskView
{

    Q_OBJECT

public:

    OrthophotoView(QWidget *parent) : TaskView(parent) {}
    ~OrthophotoView() override = default;

    virtual double gsd() const = 0;

public slots:

    virtual void setGSD(double gsd) = 0;

signals:

    void resolutionChanged(double);

};

} // namespace graphos


#endif // GRAPHOS_ORTHOPHOTO_VIEW_INTERFACE_H
