#ifndef GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/orientation/photoorientation.h"

class QStandardItemModel;

namespace graphos
{

class GroundControlPoint;
class Image;

namespace ui
{

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

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
