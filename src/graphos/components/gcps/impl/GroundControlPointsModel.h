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

#ifndef GRAPHOS_GCPS_MODEL_H
#define GRAPHOS_GCPS_MODEL_H

#include "graphos/components/gcps/GroundControlPointsModel.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{

class Project;

class GroundControlPointsModelImp
  : public GroundControlPointsModel
{

    Q_OBJECT

public:

    GroundControlPointsModelImp(Project *project,
                                QObject *parent = nullptr);
    ~GroundControlPointsModelImp() override = default;

private:

    void writeGroundControlPoints(QXmlStreamWriter &stream);

// GroundControlPointsModel interface

public:

    void loadGroundControlPoints() override;
    void loadGroundControlPoints(const GroundControlPoints &gcps) override;
    auto crs() const -> QString override;
    auto itemModelGroundControlPoints() -> QStandardItemModel* override;
    auto itemModelImagePoints() -> QStandardItemModel* override;
    auto images() const -> const Images& override;
    auto image(size_t imageId) const -> Image override;
    void addGroundControlPoint() override;
    void removeGroundControlPoint(int index) override;
    void addImagePoint(const QString &gcp,
                       size_t imageId,
                       const QPointF &point) override;
    void removeImagePoint(const QString &gcp, size_t imageId) override;
    auto points(size_t imageId) const -> std::list<std::pair<QString, QPointF>> override;
    auto groundControlPoints() const -> GroundControlPoints override;
    auto projectPath() const -> tl::Path override;
    auto reconstructionPath() const -> tl::Path override;
    void setSparseModel(const tl::Path &sparseModel) override;
    void setOffset(const tl::Path &offset) override;
    void addPhotoOrientation(size_t imageId,
                             const CameraPose &orientation) override;
    auto poses() const -> const CameraPoses& override;
    auto cameras() const -> const Cameras& override;
    auto groundPoints() const -> GroundPoints override;
    auto database() const -> tl::Path override;
    void setTransform(const tl::Matrix<double, 4, 4> &transform) override;
    void setGroundPoints(const tl::Path &groundPoints) override;
    auto updateCamera(int id, const Camera &camera) -> bool override;
    void importGroundControlPoints(const QString &file, const QString &format) override;
    void exportGroundControlPoints(const QString &file, const QString &format) override;

public slots:

    void save() override;
    void setCrs(const QString &crs) override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QStandardItemModel *mItemModelGroundControlPoints;
    QStandardItemModel *mItemModelImagePoints;
    QString mCrs;
};

} // namespace graphos

#endif // GRAPHOS_GCPS_MODEL_H
