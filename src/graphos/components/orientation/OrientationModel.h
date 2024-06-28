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

#ifndef GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
#define GRAPHOS_ORIENTATION_MODEL_INTERFACE_H

#include <unordered_map>

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"
#include "graphos/core/image.h"
#include "graphos/core/sfm/poses.h"
#include "graphos/core/sfm/OrientationReport.h"

namespace graphos
{

class Camera;

class OrientationModel
  : public Model
{
public :

    using Images = const std::unordered_map<size_t, Image>;

public:

    OrientationModel(QObject *parent = nullptr) : Model(parent) {}
    ~OrientationModel() override = default;

    virtual auto calibratedCamera() const -> bool = 0;
    virtual void setSparseModel(const tl::Path &sparseModel) = 0;
    virtual void setOffset(const tl::Path &offset) = 0;
    virtual void setGroundPoints(const tl::Path &groundPoints) = 0;
    virtual auto isPhotoOriented(size_t imageId) const -> bool = 0;
    virtual auto photoOrientation(size_t imageId) const -> CameraPose = 0;
    virtual void addPhotoOrientation(size_t imageId, const CameraPose &orientation) = 0;
    virtual auto database() const -> tl::Path = 0;
    virtual auto projectFolder() const -> tl::Path = 0;
    virtual auto gpsPositions() const -> bool = 0;
    virtual auto rtkOrientations() const -> bool = 0;
    virtual auto hasControlPoints() const -> bool = 0;
    virtual auto existReconstruction() const -> bool = 0;
    virtual void clearProject() = 0;
    virtual auto cameras() const -> const std::map<int, Camera>& = 0;
    virtual auto updateCamera(int id, const Camera& camera) -> bool = 0;
    virtual auto images() const -> const Images & = 0;
    virtual auto orientationReport() const -> OrientationReport = 0;
    virtual void setOrientationReport(const OrientationReport &orientationReport) = 0;
};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_MODEL_INTERFACE_H
