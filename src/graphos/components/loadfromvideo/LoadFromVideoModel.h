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

#ifndef GRAPHOS_LOADFROMVIDEO_MODEL_INTERFACE_H
#define GRAPHOS_LOADFROMVIDEO_MODEL_INTERFACE_H

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"
#include "graphos/core/camera/Camera.h"

namespace graphos
{

class Image;

class LoadFromVideoModel
  : public Model
{

    Q_OBJECT

public:

    LoadFromVideoModel(QObject *parent = nullptr) : Model(parent) {}
    ~LoadFromVideoModel() override = default;

    virtual auto imagesPath() const -> tl::Path = 0;
    virtual void addImage(const Image &image) = 0;
    virtual auto cameras() const -> const std::map<int, Camera>& = 0;
    virtual auto addCamera(const Camera& camera) -> int = 0;
    virtual auto cameraID(const Camera& camera) const -> int = 0;
    virtual auto cameraID(const QString& make,
                          const QString& model) const -> int = 0;

};

} // namespace graphos


#endif // GRAPHOS_LOADFROMVIDEO_MODEL_INTERFACE_H
