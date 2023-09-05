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


#ifndef GRAPHOS_LOADFROMVIDEO_PRESENTER_INTERFACE_H
#define GRAPHOS_LOADFROMVIDEO_PRESENTER_INTERFACE_H

#include "graphos/core/task/TaskPresenter.h"

namespace graphos
{

class LoadFromVideoPresenter
  : public TaskPresenter
{

    Q_OBJECT

public:

    LoadFromVideoPresenter() : TaskPresenter() {}
    ~LoadFromVideoPresenter() override = default;

public slots:

    //virtual void setImages(const QStringList &files) = 0;

protected slots:

    virtual void addImage(QString image, int cameraId) = 0;

signals:

    void frame_loaded(size_t);
};

} // namespace graphos


#endif // GRAPHOS_LOADFROMVIDEO_PRESENTER_INTERFACE_H
