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

#ifndef GRAPHOS_EXPORT_CAMERAS_TASK_H
#define GRAPHOS_EXPORT_CAMERAS_TASK_H

#include <unordered_map>

#include <QObject>

#include <tidop/core/task.h>

#include "graphos/core/image.h"

namespace graphos
{

class Camera;

class ExportCamerasTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    ExportCamerasTask(const QString &file,
                      const std::unordered_map<size_t, Image> &images,
                      const std::unordered_map<size_t, CameraPose> &poses,
                      const QString &format);
    ~ExportCamerasTask() override;

    void setQuaternionRotation(bool quaternions);

private:

    void textExport();

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

protected:

    QString mFile;
    std::unordered_map<size_t, Image> mImages;
    std::unordered_map<size_t, CameraPose> mPoses;
    QString mFormat;
    bool mQuaternions;
};

} // namespace graphos

#endif // GRAPHOS_EXPORT_CAMERAS_TASK_H
