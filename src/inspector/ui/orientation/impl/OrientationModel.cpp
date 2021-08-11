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

CameraPose OrientationModelImp::photoOrientation(const QString &imgName) const
{
  return mProject->photoOrientation(imgName);
}

void OrientationModelImp::addPhotoOrientation(const QString &imgName, const CameraPose &orientation)
{
  mProject->addPhotoOrientation(imgName, orientation);
}

QString OrientationModelImp::database() const
{
  return mProject->database();
}


QString OrientationModelImp::projectPath() const
{
  return mProject->projectFolder();
}

bool OrientationModelImp::gpsPositions() const
{
  bool bGpsOrientation = false;

  auto it = mProject->imageBegin();
  CameraPosition cameraPosition = it->cameraPosition();
  if (!cameraPosition.isEmpty())
    bGpsOrientation = true;

  return bGpsOrientation;
}

bool OrientationModelImp::rtkOrientations() const
{
  bool bRtkOrientations = false;

  auto it = mProject->imageBegin();
  CameraPosition cameraPosition = it->cameraPosition();
  if (!cameraPosition.isEmpty()){
    tl::math::Quaternion<double> q = cameraPosition.quaternion();
    if (q.x == 0 && q.y == 0 && q.z == 0 && q.w == 0)
      bRtkOrientations = false;
    else 
      bRtkOrientations = true;
  }

  return bRtkOrientations;
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
    //QString file_name = QFileInfo(path).fileName();
    CameraPosition cameraPosition = it->cameraPosition();
    if (!cameraPosition.isEmpty()) {
      std::array<double, 3> positions = {
      cameraPosition.x(),
      cameraPosition.y(),
      cameraPosition.z()};
      camera_positions[path] = positions;
    }
  }
  return camera_positions;
}

void OrientationModelImp::clearProject()
{
  mProject->clearReconstruction();
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
