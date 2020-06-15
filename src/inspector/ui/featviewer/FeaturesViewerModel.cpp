#include "FeaturesViewerModel.h"

#include "inspector/core/features/featio.h"
#include "inspector/ui/ProjectModel.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>

#include <QFileInfo>

namespace inspector
{

namespace ui
{
	
FeaturesViewerModelImp::FeaturesViewerModelImp(Project *project,
                                               QObject *parent)
  : FeaturesViewerModel(parent),
    mProject(project)
{
  this->init();
}

FeaturesViewerModelImp::~FeaturesViewerModelImp()
{

}

void FeaturesViewerModelImp::init()
{
}

void FeaturesViewerModelImp::clear()
{

}

std::vector<QString> FeaturesViewerModelImp::images() const
{
  std::vector<QString> images;
  for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++){
    images.push_back((*it).path());
  }
  return images;
}

std::vector<QPointF> FeaturesViewerModelImp::loadKeypoints(const QString &imageName)
{
  try {
    std::vector<QPointF> keyPoints;

    QString database_path = mProject->database();
    Image image = mProject->findImageByName(imageName);
    QString image_file_name = QFileInfo(image.path()).fileName();

    if (QFileInfo(database_path).exists()){

      colmap::Database database(database_path.toStdString());
      std::vector<colmap::Image> db_images = database.ReadAllImages();
      colmap::image_t image_id = 0;
      for (size_t i = 0; i < db_images.size(); i++){
        if (image_file_name.compare(db_images[i].Name().c_str()) == 0){
          image_id = db_images[i].ImageId();
          break;
        }
      }

      if (image_id > 0){
        colmap::FeatureKeypoints colmap_feature_keypoints = database.ReadKeypoints(image_id);
        size_t size = colmap_feature_keypoints.size();
        keyPoints.resize(size);
        for (size_t i = 0; i < size; i++){
          keyPoints[i].setX(static_cast<qreal>(colmap_feature_keypoints[i].x));
          keyPoints[i].setY(static_cast<qreal>(colmap_feature_keypoints[i].y));
        }

      }
    }

    return keyPoints;

  } catch (std::exception &e) {
    msgError(e.what());
    return std::vector<QPointF>();
  }
}

} // namespace ui

} // namespace inspector
