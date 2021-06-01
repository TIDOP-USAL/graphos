#ifndef INSPECTOR_ORIENTATION_MODEL_H
#define INSPECTOR_ORIENTATION_MODEL_H

#include "inspector/ui/orientation/OrientationModel.h"

namespace inspector
{

class Project;

namespace ui
{

class OrientationModelImp
  : public OrientationModel
{

public:

  OrientationModelImp(Project *project,
                      QObject *parent = nullptr);
  ~OrientationModelImp() override = default;

// OrientationModel interface

public:

  bool refinePrincipalPoint() const override;
  void setRefinePrincipalPoint(bool refine) override;
  void setSparseModel(const QString &sparseModel) override;
  void setOffset(const QString &offset) override;
  bool isPhotoOriented(const QString &imgName) const override;
  CameraPose photoOrientation(const QString &imgName) const override;
  void addPhotoOrientation(const QString &imgName, const CameraPose &orientation) override;
  QString database() const override;
  QString projectPath() const override;
  bool gpsPositions() const override;
  bool rtkOrientations() const override;
  QString reconstructionPath() const override;
  void setReconstructionPath(const QString &reconstructionPath) override;
  std::map<QString, std::array<double, 3>> cameraPositions() const override;
  void clearProject() override;

  bool updateCamera(int id, const Camera &camera) override;
  camera_iterator cameraBegin() override;
  camera_const_iterator cameraBegin() const override;
  camera_iterator cameraEnd() override;
  camera_const_iterator cameraEnd() const override;

  image_iterator imageBegin() override;
  image_const_iterator imageBegin() const override;
  image_iterator imageEnd() override;
  image_const_iterator imageEnd() const override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_MODEL_H
