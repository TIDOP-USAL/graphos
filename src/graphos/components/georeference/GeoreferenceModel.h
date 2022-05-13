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

#include "graphos/interfaces/mvp.h"
#include "graphos/core/orientation/poses.h"

class QStandardItemModel;

namespace graphos
{

class GroundControlPoint;
class Image;

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
  virtual QString projectPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual void addPhotoOrientation(size_t imageId,
                                   const CameraPose &orientation) = 0;

public slots:

  virtual void save() = 0;
  virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
