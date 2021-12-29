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

#ifndef GRAPHOS_GEOREFERENCE_MODEL_H
#define GRAPHOS_GEOREFERENCE_MODEL_H

#include "graphos/components/georeference/GeoreferenceModel.h"

//#include <tidop/geometry/entities/point.h>

class QXmlStreamWriter;
class QXmlStreamReader;

namespace graphos
{

class Project;

//TODO: mover a core

//class GroundControlPoint
//{
//
//public:
//
//  GroundControlPoint();
//  ~GroundControlPoint();
//
//  std::string name() const;
//  void setName(const std::string &name);
//  tl::Point3D point() const;
//  void setPoint(const tl::Point3D &point);
//  double x() const;
//  void setX(double x);
//  double y() const;
//  void setY(double y);
//  double z() const;
//  void setZ(double z);
//  void addImagePoint(const std::string &image, const tl::PointD &point);
//  tl::PointD imagePoint(const std::string &image);
//  bool existImagePoint(const std::string &image);
//
//protected:
//
//  std::string mName;
//  tl::Point3D mCoordinates;
//  std::map<std::string, tl::PointD> mPoints;
//};

class GeoreferenceModelImp
  : public GeoreferenceModel
{

  Q_OBJECT

public:

  GeoreferenceModelImp(Project *project,
                       QObject *parent = nullptr);
  ~GeoreferenceModelImp() override = default;

private:

  void readGroundControlPoints(QXmlStreamReader &stream);
  void readGroundControlPoint(QXmlStreamReader &stream);
  void readImagePoints(QXmlStreamReader &stream, const QString &gcp_id);
  void readImagePoint(QXmlStreamReader &stream, const QString &gcp_id);
  void writeGroundControlPoints(QXmlStreamWriter &stream);

// GeoreferenceModel interface

public:
  
  void loadGroundControlPoints() override;
  QString crs() const;
  QStandardItemModel *itemModelGroundControlPoints() override;
  QStandardItemModel *itemModelImagePoints() override;
  std::vector<QString> images() const override;
  void addGroundControlPoint() override;
  void removeGroundControlPoint(int index) override;
  void addImagePoint(const QString &gcp, const QString &image, const QPointF &point) override;
  void removeImagePoint(const QString &gcp, const QString &image) override;
  std::list<std::pair<QString, QPointF>> points(const QString &image) const override;
  std::vector<GroundControlPoint> groundControlPoints() const override;
  QString projectPath() const override;
  void setReconstructionPath(const QString &reconstructionPath) override;
  void setSparseModel(const QString &sparseModel) override;
  void setOffset(const QString &offset) override;
  void addPhotoOrientation(const QString &imgName, 
                           const CameraPose &orientation) override;
  image_iterator imageBegin();
  image_const_iterator imageBegin() const;
  image_iterator imageEnd();
  image_const_iterator imageEnd() const;

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
