#include "MatchViewerModel.h"

#include "inspector/core/project.h"

#include <colmap/base/database.h>

#include <QImageReader>
#include <QFileInfo>
#include <QSettings>

namespace inspector
{

namespace ui
{

MatchViewerModelImp::MatchViewerModelImp(Project *project,
                                         QObject *parent)
  : MatchViewerModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, 
                            QSettings::UserScope, 
                            "TIDOP", 
                            "Inspector"))
{
  this->init();
}

MatchViewerModelImp::~MatchViewerModelImp()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }
}

QString MatchViewerModelImp::viewerBGColor() const
{
  return mSettings->value("MatchesViewer/BGColor", "#dcdcdc").toString();
}

int MatchViewerModelImp::viewerMarkerType() const
{
  return mSettings->value("MatchesViewer/MarkerType", 0).toInt();
}

int MatchViewerModelImp::viewerMarkerSize() const
{
  return mSettings->value("MatchesViewer/MarkerSize", 20).toInt();
}

int MatchViewerModelImp::viewerMarkerWidth() const
{
  return mSettings->value("MatchesViewer/MarkerWidth", 2).toInt();
}

QString MatchViewerModelImp::viewerMarkerColor() const
{
  return mSettings->value("MatchesViewer/MarkerColor", "#00aa00").toString();
}

int MatchViewerModelImp::viewerSelectMarkerWidth() const
{
  return mSettings->value("MatchesViewer/SelectMarkerWidth", 2).toInt();
}

QString MatchViewerModelImp::viewerSelectMarkerColor() const
{
  return mSettings->value("MatchesViewer/SelectMarkerColor", "#e5097e").toString();
}

QString MatchViewerModelImp::viewerLineColor() const
{
  return mSettings->value("MatchesViewer/LineColor", "#0000ff").toString();
}

int MatchViewerModelImp::viewerLineWidth() const
{
  return mSettings->value("MatchesViewer/LineWidth", 2).toInt();
}

void MatchViewerModelImp::init()
{
}

std::vector<QString> MatchViewerModelImp::images() const
{
  std::vector<QString> images;
  for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++){
    images.push_back((*it).path());
  }
  return images;
}

std::vector<QString> MatchViewerModelImp::imagePairs(const QString &imageName) const
{
  std::vector<QString> pair_names = mProject->matchesPairs(imageName);
  std::vector<QString> image_pairs;
  for (auto &image_name : pair_names){
    image_pairs.push_back(mProject->findImageByName(image_name).path());
  }
  return image_pairs;
}

std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> MatchViewerModelImp::loadMatches(const QString &imgName1, const QString &imgName2) const
{
  std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> matches;

  QString database_path = mProject->database();

  Image imageLeft = mProject->findImageByName(imgName1);
  Image imageRight = mProject->findImageByName(imgName2);
  QString image_left_file_name = QFileInfo(imageLeft.path()).fileName();
  QString image_right_file_name = QFileInfo(imageRight.path()).fileName();

  if (QFileInfo(database_path).exists()){
    colmap::Database database(database_path.toStdString());

    std::vector<colmap::Image> db_images = database.ReadAllImages();

    colmap::image_t image_id_left = 0;
    for (size_t i = 0; i < db_images.size(); i++){
      if (image_left_file_name.compare(db_images[i].Name().c_str()) == 0){
        image_id_left = db_images[i].ImageId();
        break;
      }
    }

    colmap::image_t image_id_right = 0;
    for (size_t i = 0; i < db_images.size(); i++){
      if (image_right_file_name.compare(db_images[i].Name().c_str()) == 0){
        image_id_right = db_images[i].ImageId();
        break;
      }
    }

    if (image_id_right != 0 && image_id_right != 0) {

      colmap::FeatureKeypoints kp_1 = database.ReadKeypoints(image_id_left);
      colmap::FeatureKeypoints kp_2 = database.ReadKeypoints(image_id_right);

      colmap::TwoViewGeometry two_view_colmap = database.ReadTwoViewGeometry(image_id_left, image_id_right);
      colmap::FeatureMatches inlier_matches = two_view_colmap.inlier_matches;
      colmap::FeatureMatch match_colmap;
      for (size_t i = 0; i < inlier_matches.size(); i++) {
        match_colmap = inlier_matches[i];
        QPointF left_point(static_cast<qreal>(kp_1[match_colmap.point2D_idx1].x),
                           static_cast<qreal>(kp_1[match_colmap.point2D_idx1].y));
        QPointF right_point(static_cast<qreal>(kp_2[match_colmap.point2D_idx2].x),
                            static_cast<qreal>(kp_2[match_colmap.point2D_idx2].y));
        matches.push_back(std::make_tuple(i,
                          match_colmap.point2D_idx1,
                          left_point,
                          match_colmap.point2D_idx1,
                          right_point));
      }
    }

  }

  return matches;
}

void MatchViewerModelImp::deleteMatch(const QString &imgName1, const QString &imgName2, int query_id, int train_id)
{
//  QString imgPath1 = mProjectModel->findImageByName(imgName1)->path();
//  QString imgPath2 = mProjectModel->findImageByName(imgName2)->path();

//  if (QFileInfo(imgPath1).exists() == true && QFileInfo(imgPath2).exists() == true) {
//    if (std::shared_ptr<Session> session = mProjectModel->findSession(mSession)){
//      std::vector<std::pair<QString, QString>> matches = session->matches(imgName1);

//      if (!matches.empty()){
//        for (auto &m : matches){
//          if (m.first.compare(imgName2) == 0){

//            std::unique_ptr<MatchesReader> matchesReader = MatchesReaderFactory::createReader(m.second);
//            matchesReader->read();
//            std::vector<cv::DMatch> good_matches = matchesReader->goodMatches();
//            std::vector<cv::DMatch> wrong_matches = matchesReader->wrongMatches();
//            matchesReader.reset();

//            for (size_t i = 0; i < good_matches.size(); i++){
//              if (good_matches[i].queryIdx == query_id &&
//                  good_matches[i].trainIdx == train_id){
//                wrong_matches.push_back(good_matches[i]);
//                good_matches.erase(good_matches.begin()+static_cast<long long>(i));
//                break;
//              }
//            }

//            //matchesWrite(m.second, good_matches, wrong_matches);
//            std::unique_ptr<MatchesWriter> matchesWriter = MatchesWriterFactory::createWriter(m.second);
//            matchesWriter->setGoodMatches(good_matches);
//            matchesWriter->setWrongMatches(wrong_matches);
//            matchesWriter->write();

//            break;
//          }
//        }
//      }
//    }
//  }
}

//void MatchViewerModelImp::loadPassPoints()
//{
//  if (std::shared_ptr<Session> session = mProjectModel->findSession(mSession)){
//    passPointsRead(session->passPoints(), mPassPoints);
//  } else {
//    mPassPoints.clear();
//  }
//}

void MatchViewerModelImp::clear()
{
}

} // namespace ui

} // namespace inspector


