#include "OrientationModel.h"

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

} // namespace ui

} // namespace inspector
