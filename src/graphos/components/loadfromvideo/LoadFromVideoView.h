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


#ifndef GRAPHOS_LOADFROMVIDEO_VIEW_INTERFACE_H
#define GRAPHOS_LOADFROMVIDEO_VIEW_INTERFACE_H


#include "graphos/core/task/TaskView.h"

namespace graphos
{

class LoadFromVideoView
  : public TaskView
{

    Q_OBJECT

public:

    LoadFromVideoView(QWidget *parent) : TaskView(parent) {}
    ~LoadFromVideoView() override = default;

    virtual auto video() const -> QString = 0;
    virtual auto skipFrames() const -> int = 0;
    virtual auto videoIni() const -> int = 0;
    virtual auto videoEnd() const -> int = 0;

public slots:

    virtual void setSkipFrames(int skipFrames) = 0;

signals:

    void skip_frames_changed(int);
    void start_video_changed(int);
    void end_video_changed(int);
};

} // namespace graphos


#endif // GRAPHOS_LOADFROMVIDEO_VIEW_INTERFACE_H
