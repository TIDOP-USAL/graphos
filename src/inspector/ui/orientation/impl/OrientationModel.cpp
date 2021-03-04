#include "OrientationModel.h"

#include "inspector/core/project.h"

#include <QFileInfo>

namespace inspector
{

namespace ui
{

OrientationModelImp::OrientationModelImp(Project *project,
                                         QObject *parent)
  : OrientationModel(parent),
    mProject(project)
{
  this->init();
}


void OrientationModelImp::init()
{
}

void OrientationModelImp::clear()
{
  mProject->clearReconstruction();
}

bool OrientationModelImp::refinePrincipalPoint() const
{
  return mProject->refinePrincipalPoint();
}

void OrientationModelImp::setRefinePrincipalPoint(bool refine)
{
  mProject->setRefinePrincipalPoint(refine);
}

void OrientationModelImp::setSparseModel(const QString &sparseModel)
{
  mProject->setSparseModel(sparseModel);
}

void OrientationModelImp::setOffset(const QString &offset)
{
  mProject->setOffset(offset);
}

bool OrientationModelImp::isPhotoOriented(const QString &imgName) const
{
  return mProject->isPhotoOriented(imgName);
}

PhotoOrientation OrientationModelImp::photoOrientation(const QString &imgName) const
{
  return mProject->photoOrientation(imgName);
}

void OrientationModelImp::addPhotoOrientation(const QString &imgName, const PhotoOrientation &orientation)
{
  mProject->addPhotoOrientation(imgName, orientation);
}

QString OrientationModelImp::database() const
{
  return mProject->database();
}

QString OrientationModelImp::imagePath() const
{
  return mProject->imageDirectory();
}

QString OrientationModelImp::projectPath() const
{
  return mProject->projectFolder();
}

bool OrientationModelImp::gpsOrientation() const
{
  bool bGpsOrientation = false;

  auto it = mProject->imageBegin();
  CameraPosition cameraPosition = it->cameraPosition();
  if (!cameraPosition.isEmpty())
    bGpsOrientation = true;

  return bGpsOrientation;
}

QString OrientationModelImp::reconstructionPath() const
{
  return mProject->reconstructionPath();
}

void OrientationModelImp::setReconstructionPath(const QString &reconstructionPath)
{
  mProject->setReconstructionPath(reconstructionPath);
}

std::map<QString, std::array<double, 3>> OrientationModelImp::cameraPositions() const
{
  std::map<QString, std::array<double, 3>> camera_positions;
  for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++) {
    QString path = it->path();
    QString file_name = QFileInfo(path).fileName();
    CameraPosition cameraPosition = it->cameraPosition();
    std::array<double, 3> positions = {
    cameraPosition.x(),
    cameraPosition.y(),
    cameraPosition.z()};
    camera_positions[file_name] = positions;
  }
  return camera_positions;
}

bool OrientationModelImp::updateCamera(int id, const Camera &camera)
{
  return mProject->updateCamera(id, camera);
}

OrientationModel::camera_iterator OrientationModelImp::cameraBegin()
{
  return mProject->cameraBegin();
}

OrientationModel::camera_const_iterator OrientationModelImp::cameraBegin() const
{
  return mProject->cameraBegin();
}

OrientationModel::camera_iterator OrientationModelImp::cameraEnd()
{
  return mProject->cameraEnd();
}

OrientationModel::camera_const_iterator OrientationModelImp::cameraEnd() const
{
  return mProject->cameraEnd();
}

OrientationModel::image_iterator OrientationModelImp::imageBegin()
{
  return mProject->imageBegin();
}

OrientationModel::image_const_iterator OrientationModelImp::imageBegin() const
{
  return mProject->imageBegin();
}

OrientationModel::image_iterator OrientationModelImp::imageEnd()
{
  return mProject->imageEnd();
}

OrientationModel::image_const_iterator OrientationModelImp::imageEnd() const
{
  return mProject->imageEnd();
}

} // namespace ui

} // namespace inspector
