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

#ifndef GRAPHOS_UNDISTORTIMAGES_MODEL_INTERFACE_H
#define GRAPHOS_UNDISTORTIMAGES_MODEL_INTERFACE_H

#include <unordered_map>

#include <QString>

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"

namespace graphos
{

class Camera;
class Image;

class UndistortImagesModel
  : public Model
{

    Q_OBJECT

public:

    using Images = std::unordered_map<size_t, Image>;
    using Cameras = std::map<int, Camera>;

public:

    UndistortImagesModel(QObject *parent = nullptr) : Model(parent) {}
    ~UndistortImagesModel() override = default;

    virtual auto images() const -> const Images& = 0;
    virtual auto cameras() const -> const Cameras & = 0;
    virtual auto useCuda() const -> bool = 0;
    virtual auto projectFolder() const -> tl::Path = 0;

public slots:

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

} // namespace graphos


#endif // GRAPHOS_UNDISTORTIMAGES_MODEL_INTERFACE_H
