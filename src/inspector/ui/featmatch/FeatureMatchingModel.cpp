#include "FeatureMatchingModel.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>

#include <QFileInfo>

namespace inspector
{

namespace ui
{

FeatureMatchingModelImp::FeatureMatchingModelImp(Project *project,
                                                 QObject *parent)
  : FeatureMatchingModel(parent),
    mProject(project)
{
  this->init();
}

FeatureMatchingModelImp::~FeatureMatchingModelImp()
{
}

void FeatureMatchingModelImp::init()
{
}

void FeatureMatchingModelImp::clear()
{
  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  database.ClearMatches();
  database.ClearTwoViewGeometries();
  database.Close();
  mProject->removeMatchesPair();
}

std::shared_ptr<FeatureMatching> FeatureMatchingModelImp::featureMatching() const
{
  return mProject->featureMatching();
}

void FeatureMatchingModelImp::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
  mProject->setFeatureMatching(featureMatching);
}

QString FeatureMatchingModelImp::database() const
{
  return mProject->database();
}

bool FeatureMatchingModelImp::spatialMatching() const
{
  bool bSpatialMatching = false;

  auto it = mProject->imageBegin();
  CameraPosition cameraPosition = it->cameraPosition();
  if (!cameraPosition.isEmpty())
    bSpatialMatching = true;

  return bSpatialMatching;
}

void FeatureMatchingModelImp::writeMatchPairs()
{
  QString database_file = mProject->database();
  colmap::Database database(database_file.toStdString());
  std::vector<colmap::Image> db_images = database.ReadAllImages();
  colmap::image_t image_id_l = 0;
  colmap::image_t image_id_r = 0;
  for (size_t i = 0; i < db_images.size(); i++){
    image_id_l = db_images[i].ImageId();
    for (size_t j = 0; j < i; j++){
      image_id_r = db_images[j].ImageId();

      colmap::FeatureMatches matches = database.ReadMatches(image_id_l, image_id_r);
      if (matches.size() > 0){
        QString path_left = QFileInfo(db_images[i].Name().c_str()).baseName();
        QString path_right = QFileInfo(db_images[j].Name().c_str()).baseName();
        mProject->addMatchesPair(path_left, path_right);
      }
    }
  }
}

} // End namespace ui

} // End namespace inspector
