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

#ifndef GRAPHOS_EXPORT_CAMERAS_MODEL_INTERFACE_H
#define GRAPHOS_EXPORT_CAMERAS_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

#include <unordered_map>
#include <tidop/geometry/entities/point.h>

namespace graphos
{

class Image;
class CameraPose;

class ExportCamerasModel
  : public Model
{

    Q_OBJECT

public:

    ExportCamerasModel(QObject *parent = nullptr) : Model(parent) {}

    virtual auto poses() const -> const std::unordered_map<size_t, CameraPose>& = 0;
    virtual auto images() const -> const std::unordered_map<size_t, Image>& = 0;
    virtual auto offset() const -> tl::Point3<double> = 0;
};

} // namespace graphos

#endif // GRAPHOS_EXPORT_CAMERAS_MODEL_INTERFACE_H
