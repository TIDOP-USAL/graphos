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

#ifndef GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H

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

class GeoreferenceModel
  : public Model
{

    Q_OBJECT

public:

    GeoreferenceModel(QObject *parent = nullptr) : Model(parent) {}
    ~GeoreferenceModel() override = default;

    virtual void loadGroundControlPoints() = 0;
    virtual QString crs() const = 0;
    virtual QStandardItemModel *itemModelGroundControlPoints() = 0;
    virtual QStandardItemModel *itemModelImagePoints() = 0;
    virtual const std::unordered_map<size_t, Image> &images() const = 0;
    virtual Image image(size_t imageId) const = 0;
    virtual void addGroundControlPoint() = 0;
    virtual void removeGroundControlPoint(int index) = 0;
    virtual void addImagePoint(const QString &gcp, size_t imageId, const QPointF &point) = 0;
    virtual void removeImagePoint(const QString &gcp, size_t imageId) = 0;
    virtual std::list<std::pair<QString, QPointF>> points(size_t imageId) const = 0;
    virtual std::vector<GroundControlPoint> groundControlPoints() const = 0;
    virtual tl::Path projectPath() const = 0;
    virtual tl::Path reconstructionPath() const = 0;
    virtual void setReconstructionPath(const tl::Path &reconstructionPath) = 0;
    virtual void setSparseModel(const tl::Path &sparseModel) = 0;
    virtual void setOffset(const tl::Path &offset) = 0;
    virtual void addPhotoOrientation(size_t imageId,
                                     const CameraPose &orientation) = 0;
    virtual const std::unordered_map<size_t, CameraPose> &poses() const = 0;
    virtual const std::map<int, Camera> &cameras() const = 0;
    virtual std::vector<GroundPoint> groundPoints() const = 0;
    virtual tl::Path database() const = 0;
    virtual void setTransform(const tl::Matrix<double, 4, 4> &trasnform) = 0;
    virtual void setGroundPoints(const tl::Path &groundPoints) = 0;
    virtual bool updateCamera(int id, const Camera &camera) = 0;

public slots:

    virtual void save() = 0;
    virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
