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

#ifndef GRAPHOS_GEOREFERENCE_MODEL_H
#define GRAPHOS_GEOREFERENCE_MODEL_H

#include "graphos/components/georeference/GeoreferenceModel.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{

class Project;

class GeoreferenceModelImp
  : public GeoreferenceModel
{

    Q_OBJECT

public:

    GeoreferenceModelImp(Project *project,
                         QObject *parent = nullptr);
    ~GeoreferenceModelImp() override = default;

private:

    //void readGroundControlPoints(QXmlStreamReader &stream);
    //void readGroundControlPoint(QXmlStreamReader &stream);
    //void readImagePoints(QXmlStreamReader &stream, const QString &gcp_id);
    //void readImagePoint(QXmlStreamReader &stream, const QString &gcp_id);
    void writeGroundControlPoints(QXmlStreamWriter &stream);

// GeoreferenceModel interface

public:

    void loadGroundControlPoints() override;
    QString crs() const;
    QStandardItemModel *itemModelGroundControlPoints() override;
    QStandardItemModel *itemModelImagePoints() override;
    const std::unordered_map<size_t, Image> &images() const override;
    Image image(size_t imageId) const override;
    void addGroundControlPoint() override;
    void removeGroundControlPoint(int index) override;
    void addImagePoint(const QString &gcp,
                       size_t imageId,
                       const QPointF &point) override;
    void removeImagePoint(const QString &gcp, size_t imageId) override;
    std::list<std::pair<QString, QPointF>> points(size_t imageId) const override;
    std::vector<GroundControlPoint> groundControlPoints() const override;
    tl::Path projectPath() const override;
    tl::Path reconstructionPath() const override;
    //void setReconstructionPath(const tl::Path &reconstructionPath) override;
    void setSparseModel(const tl::Path &sparseModel) override;
    void setOffset(const tl::Path &offset) override;
    void addPhotoOrientation(size_t imageId,
                             const CameraPose &orientation) override;
    const std::unordered_map<size_t, CameraPose> &poses() const override;
    const std::map<int, Camera> &cameras() const override;
    std::vector<GroundPoint> groundPoints() const override;
    tl::Path database() const override;
    void setTransform(const tl::Matrix<double, 4, 4> &transform) override;

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
    //std::vector<GroundControlPoint> mGroundControlPoints;
    QStandardItemModel *mItemModelGroundControlPoints;
    QStandardItemModel *mItemModelImagePoints;
    QString mCrs;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_H
