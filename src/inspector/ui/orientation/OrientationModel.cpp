#include "OrientationModel.h"

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
  if (it->longitudeExif() != 0.0 &&
      it->latitudeExif() != 0.0)
    bGpsOrientation = true;

  return bGpsOrientation;
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
    std::array<double, 3> positions = {it->longitudeExif(), it->latitudeExif(), it->altitudeExif()};
    camera_positions[file_name] = positions;
  }
  return camera_positions;
}

} // namespace ui

} // namespace inspector
