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

#ifndef GRAPHOS_LOADER_IMAGES_TASK_H
#define GRAPHOS_LOADER_IMAGES_TASK_H

#include <QObject>

#include <tidop/core/task/Task.h>

#include "graphos/core/image.h"

namespace tl
{
class RasterReader;
class Process;
class Crs;
}

namespace graphos
{


class Camera;

class LoadImagesTask
  : public QObject,
    public tl::Task
{

    Q_OBJECT

public:

    LoadImagesTask(std::vector<Image> *images,
                   std::vector<Camera> *cameras,
                   std::string cameraType);
    ~LoadImagesTask() override;

signals:

    void imageAdded(int, int);

private:

    auto existCamera(const QString &make, 
                     const QString &model,
                     const QString &serialNumber = "", 
                     const QString &bandName = "") const -> bool;
    auto findCamera(const QString &make, 
                    const QString &model, 
                    const QString &serialNumber = "", 
                    const QString &bandName = "") const -> int;
    void loadImage(size_t imageId);
    auto loadCamera(const tl::RasterReader &imageReader) -> int;
    auto parseFocal(const std::string &focal, double def) -> double;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

protected:

    std::vector<Image> *mImages;
    std::vector<Camera> *mCameras;
    QString mDatabaseCamerasPath;
    std::string mCameraType;
};

} // namespace graphos

#endif // GRAPHOS_LOADER_IMAGES_TASK_H
