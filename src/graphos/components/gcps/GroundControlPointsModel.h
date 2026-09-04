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

#include <tidop/core/base/Path.h>
#include <tidop/math/algebra/matrix/Matrix.h>

#include "graphos/framework/mvp/Model.h"
#include "graphos/core/orientation/CameraPose.h"

class QStandardItemModel;

namespace graphos
{

class GroundControlPoint;
class ImageRepository;
class CameraPosesRepository;
class CameraRepository;
class Camera;
class CameraPose;
class GroundPoint;

class GroundControlPointsModel
  : public Model
{

    Q_OBJECT

public:

    using GroundControlPoints = std::vector<GroundControlPoint>;

public:

    GroundControlPointsModel(QObject *parent = nullptr) : Model(parent) {}
    ~GroundControlPointsModel() override = default;

    virtual void loadGroundControlPoints() = 0;
    virtual void loadGroundControlPoints(const GroundControlPoints &gcps) = 0;
    virtual auto crs() const -> QString = 0;
    virtual auto itemModelGroundControlPoints() -> QStandardItemModel* = 0;
    virtual auto itemModelImagePoints() -> QStandardItemModel* = 0;
    virtual auto images() const -> const ImageRepository & = 0;
    virtual void addGroundControlPoint() = 0;
    virtual void removeGroundControlPoint(int index) = 0;
    virtual void addImagePoint(const QString &gcp, size_t imageId, const QPointF &point) = 0;
    virtual void removeImagePoint(const QString &gcp, size_t imageId) = 0;
    virtual auto points(size_t imageId) const -> std::list<std::pair<QString, QPointF>> = 0;
    virtual auto groundControlPoints() const -> GroundControlPoints = 0;
    virtual auto projectPath() const -> tl::Path = 0;
    virtual void setEnuCrs(const QString &enuCrs) = 0;
    virtual void addPhotoOrientation(size_t imageId, const CameraPose &orientation) = 0;
    virtual auto poses() const -> const CameraPosesRepository & = 0;
    virtual auto cameras() const -> const CameraRepository & = 0;
    virtual auto database() const -> tl::Path = 0;
    virtual void setTransform(const tl::Matrix<double, 4, 4> &trasnform) = 0;
    virtual auto updateCamera(int id, const Camera& camera) -> bool = 0;
    virtual void importGroundControlPoints(const QString &file, const QString &format) = 0;
    virtual void exportGroundControlPoints(const QString &file, const QString &format) = 0;

public slots:

    virtual void save() = 0;
    virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos
