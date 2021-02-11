#ifndef INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/orientation/photoorientation.h"

class QStandardItemModel;

namespace inspector
{

class GroundControlPoint;

namespace ui
{

class GeoreferenceModel
  : public IModel
{

  Q_OBJECT

public:

  GeoreferenceModel(QObject *parent = nullptr) : IModel(parent) {}
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
                                   const PhotoOrientation &orientation) = 0;

public slots:

  virtual void save() = 0;
  virtual void setCrs(const QString &crs) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H
