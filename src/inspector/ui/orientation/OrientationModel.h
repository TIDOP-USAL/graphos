#ifndef INSPECTOR_ORIENTATION_MODEL_INTERFACE_H
#define INSPECTOR_ORIENTATION_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/camera.h"
#include "inspector/core/image.h"
#include "inspector/core/orientation/photoorientation.h"

namespace inspector
{

namespace ui
{

class OrientationModel
  : public IModel
{

public:

  typedef std::map<int, Camera>::iterator camera_iterator;
  typedef std::map<int, Camera>::const_iterator camera_const_iterator;
  typedef std::vector<Image>::iterator image_iterator;
  typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  OrientationModel(QObject *parent = nullptr) : IModel(parent) {}
  ~OrientationModel() override = default;

  virtual bool refinePrincipalPoint() const = 0;
  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual bool isPhotoOriented(const QString &imgName) const = 0;
  virtual PhotoOrientation photoOrientation(const QString &imgName) const = 0;
  virtual void addPhotoOrientation(const QString &imgName, 
                                   const PhotoOrientation &orientation) = 0;
  virtual QString database() const = 0;
  virtual QString imagePath() const = 0;
  virtual QString projectPath() const = 0;
  virtual bool gpsPositions() const = 0;
  virtual bool rtkOrientations() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual std::map<QString, std::array<double, 3>> cameraPositions() const = 0;
  virtual void clearProject() = 0;

  virtual bool updateCamera(int id, const Camera &camera) = 0;
  virtual camera_iterator cameraBegin() = 0;
  virtual camera_const_iterator cameraBegin() const = 0;
  virtual camera_iterator cameraEnd() = 0;
  virtual camera_const_iterator cameraEnd() const = 0;

  virtual image_iterator imageBegin() = 0;
  virtual image_const_iterator imageBegin() const = 0;
  virtual image_iterator imageEnd() = 0;
  virtual image_const_iterator imageEnd() const = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_MODEL_INTERFACE_H
