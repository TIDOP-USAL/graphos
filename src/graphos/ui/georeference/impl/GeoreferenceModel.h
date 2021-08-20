#ifndef GRAPHOS_GEOREFERENCE_MODEL_H
#define GRAPHOS_GEOREFERENCE_MODEL_H

#include "graphos/ui/georeference/GeoreferenceModel.h"

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

namespace ui
{

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

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_H
