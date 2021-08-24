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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/orientation/photoorientation.h"

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

  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  GeoreferenceModel(QObject *parent = nullptr) : Model(parent) {}
  ~GeoreferenceModel() override = default;

  virtual void loadGroundControlPoints() = 0;
  virtual QString crs() const = 0;
  virtual QStandardItemModel *itemModelGroundControlPoints() = 0;
  virtual QStandardItemModel *itemModelImagePoints() = 0;
  virtual std::vector<QString> images() const = 0;
  virtual void addGroundControlPoint() = 0;
  virtual void removeGroundControlPoint(int index) = 0;
  virtual void addImagePoint(const QString &gcp, const QString &image, const QPointF &point) = 0;
  virtual void removeImagePoint(const QString &gcp, const QString &image) = 0;
  virtual std::list<std::pair<QString, QPointF>> points(const QString &image) const = 0;
  virtual std::vector<GroundControlPoint> groundControlPoints() const = 0;
  virtual QString projectPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual void addPhotoOrientation(const QString &imgName, 
                                   const CameraPose &orientation) = 0;
  virtual image_iterator imageBegin() = 0;
  virtual image_const_iterator imageBegin() const = 0;
  virtual image_iterator imageEnd() = 0;
  virtual image_const_iterator imageEnd() const = 0;

public slots:

  virtual void save() = 0;
  virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
