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

#ifndef GRAPHOS_GCPS_MODEL_INTERFACE_H
#define GRAPHOS_GCPS_MODEL_INTERFACE_H

#include <unordered_map>

#include <tidop/core/path.h>
#include <tidop/math/algebra/matrix.h>

#include "graphos/core/mvp.h"
#include "graphos/core/sfm/poses.h"

class QStandardItemModel;

namespace graphos
{

class GroundControlPoint;
class Image;
class Camera;
class CameraPose;
class GroundPoint;

class GroundControlPointsModel
  : public Model
{

    Q_OBJECT

public:

    using Images = std::unordered_map<size_t, Image>;
    using CameraPoses = std::unordered_map<size_t, CameraPose>;
    using Cameras = std::map<int, Camera>;
    using GroundControlPoints = std::vector<GroundControlPoint>;
    using GroundPoints = std::vector<GroundPoint>;

public:

    GroundControlPointsModel(QObject *parent = nullptr) : Model(parent) {}
    ~GroundControlPointsModel() override = default;

    virtual void loadGroundControlPoints() = 0;
    virtual void loadGroundControlPoints(const GroundControlPoints &gcps) = 0;
    virtual auto crs() const -> QString = 0;
    virtual auto itemModelGroundControlPoints() -> QStandardItemModel* = 0;
    virtual auto itemModelImagePoints() -> QStandardItemModel* = 0;
    virtual auto images() const -> const Images & = 0;
    virtual auto image(size_t imageId) const -> Image = 0;
    virtual void addGroundControlPoint() = 0;
    virtual void removeGroundControlPoint(int index) = 0;
    virtual void addImagePoint(const QString &gcp, size_t imageId, const QPointF &point) = 0;
    virtual void removeImagePoint(const QString &gcp, size_t imageId) = 0;
    virtual auto points(size_t imageId) const -> std::list<std::pair<QString, QPointF>> = 0;
    virtual auto groundControlPoints() const -> GroundControlPoints = 0;
    virtual auto projectPath() const -> tl::Path = 0;
    virtual auto reconstructionPath() const -> tl::Path = 0;
    virtual void setSparseModel(const tl::Path &sparseModel) = 0;
    virtual void setOffset(const tl::Path &offset) = 0;
    virtual void addPhotoOrientation(size_t imageId, const CameraPose &orientation) = 0;
    virtual auto poses() const -> const CameraPoses & = 0;
    virtual auto cameras() const -> const Cameras & = 0;
    virtual auto groundPoints() const -> GroundPoints = 0;
    virtual auto database() const -> tl::Path = 0;
    virtual void setTransform(const tl::Matrix<double, 4, 4> &trasnform) = 0;
    virtual void setGroundPoints(const tl::Path &groundPoints) = 0;
    virtual auto updateCamera(int id, const Camera& camera) -> bool = 0;
    virtual void importGroundControlPoints(const QString &file, const QString &format) = 0;
    virtual void exportGroundControlPoints(const QString &file, const QString &format) = 0;

public slots:

    virtual void save() = 0;
    virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos

#endif // GRAPHOS_GCPS_MODEL_INTERFACE_H
