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

#pragma once

#include <string_view>

#include <QObject>

#include <tidop/core/task/Task.h>

#include "graphos/core/image/Image.h"

namespace tl
{
class RasterReader;
class Process;
class Crs;
}

namespace graphos
{


class CameraRepository;

class LoadImagesTask
  : public QObject,
    public tl::Task
{

    Q_OBJECT

public:

    LoadImagesTask(std::vector<Image> &images,
                   CameraRepository &cameraRepo,
                   std::string cameraType);
    ~LoadImagesTask() override = default;

    TL_DISABLE_COPY(LoadImagesTask)
    TL_DISABLE_MOVE(LoadImagesTask)

signals:

    void imageAdded(int, int);

private:

    void loadImage(size_t imageIndex);
    [[nodiscard]]
    auto loadCamera(const tl::RasterReader &imageReader) -> int;
    [[nodiscard]]
    auto parseFocal(std::string_view focal, double def) -> double;

// tl::Task interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

protected:

    std::vector<Image> &mImages;
    CameraRepository &mCameraRepo;
    QString mDatabaseCamerasPath;
    std::string mCameraType;
};

} // namespace graphos
