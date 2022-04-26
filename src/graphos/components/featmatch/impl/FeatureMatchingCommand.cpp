/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "FeatureMatchingCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/features/matching.h"
#include "graphos/core/project.h"
#include "graphos/components/featmatch/impl/FeatureMatchingProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

#include <colmap/feature/matching.h>
#include <colmap/base/database.h>

#include <QFileInfo>

using namespace tl;

namespace graphos
{

FeatureMatchingCommand::FeatureMatchingCommand()
  : Command("featmatch", "Feature Matching"),
    mProjectFile(""),
    mDisableCuda(false),
    mForceExhaustiveMatching(false)
{
  TL_TODO("Esto se tiene que leer de settings para establecer los valores por defecto")
  FeatureMatchingProperties featureMatchingProperties;

  mRatio = featureMatchingProperties.ratio();
  mDistance = featureMatchingProperties.distance();
  mMaxError = featureMatchingProperties.maxError();
  mConfidence = featureMatchingProperties.confidence();
  mCrossCheck = featureMatchingProperties.crossCheck();

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentDoubleOptional("ratio", 'r', std::string("Ratio (default = ").append(std::to_string(mRatio)).append(")"), &mRatio));
  this->push_back(CreateArgumentDoubleOptional("distance", 'd', std::string("Distance (default = ").append(std::to_string(mDistance)).append(")"), &mDistance));
  this->push_back(CreateArgumentDoubleOptional("max_error", 'e', std::string("Maximun error (default = ").append(std::to_string(mMaxError)).append(")"), &mMaxError));
  this->push_back(CreateArgumentDoubleOptional("confidence", 'c', std::string("Confidence (default = ").append(std::to_string(mConfidence)).append(")"), &mConfidence));
  this->push_back(CreateArgumentBooleanOptional("cross_check", 'x', std::string("If true, cross checking is enabled (default = ").append(mCrossCheck ? "true)" : "false)"), &mCrossCheck));
  this->push_back(CreateArgumentBooleanOptional("exhaustive_matching", "Force exhaustive matching (default = false)", &mForceExhaustiveMatching));

#ifdef HAVE_CUDA
  tl::MessageManager::instance().pause();
  bool cuda_enabled = cudaEnabled(10.0, 3.0);
  tl::MessageManager::instance().resume();
  if(cuda_enabled)
    this->push_back(CreateArgumentBooleanOptional("disable_cuda", "If true disable CUDA (default = false)", &mDisableCuda));
  else mDisableCuda = true;
#else
  mDisableCuda = true;
#endif //HAVE_CUDA

  this->addExample("featmatch --p 253/253.xml");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureMatchingCommand::~FeatureMatchingCommand()
{

}

bool FeatureMatchingCommand::run()
{
  bool r = false;

  QString file_path;
  QString project_path;

  try {

    TL_ASSERT(mProjectFile.exists(), "Project doesn't exist")
    TL_ASSERT(mProjectFile.isFile(), "Project file doesn't exist")

    QString project_file = QString::fromStdWString(mProjectFile.toWString());
    ProjectImp project;
    project.load(project_file);
    QString database_file = project.database();

    {
      colmap::Database database(database_file.toStdString());
      database.ClearMatches();
      database.ClearTwoViewGeometries();
      database.Close();
      project.removeMatchesPair();
    }


    std::shared_ptr<FeatureMatching> feature_matching_properties = std::make_shared<FeatureMatchingProperties>();
    feature_matching_properties->setRatio(mRatio);
    feature_matching_properties->setDistance(mDistance);
    feature_matching_properties->setMaxError(mMaxError);
    feature_matching_properties->setConfidence(mConfidence);
    feature_matching_properties->enableCrossCheck(mCrossCheck);

    bool spatial_matching = false;

    if (!mForceExhaustiveMatching) {
      auto it = project.imageBegin();
      if (it != project.imageEnd()) {
        CameraPose cameraPosition = it->cameraPose();
        if (!cameraPosition.isEmpty())
          spatial_matching = true;
      }
    }

    FeatureMatchingProcess featmatching_process(project.database(),
                                                !mDisableCuda,
                                                spatial_matching,
                                                feature_matching_properties);

    featmatching_process.run();

    {
      colmap::Database database(database_file.toStdString());
      std::vector<colmap::Image> db_images = database.ReadAllImages();
      colmap::image_t image_id_l = 0;
      colmap::image_t image_id_r = 0;
      for (size_t i = 0; i < db_images.size(); i++) {
        image_id_l = db_images[i].ImageId();
        for (size_t j = 0; j < i; j++) {
          image_id_r = db_images[j].ImageId();

          colmap::FeatureMatches matches = database.ReadMatches(image_id_l, image_id_r);
          if (matches.size() > 0) {
            QString path_left = QFileInfo(QString::fromStdString(db_images[i].Name())).baseName();
            QString path_right = QFileInfo(QString::fromStdString(db_images[j].Name())).baseName();
            project.addMatchesPair(path_left, path_right);
          }
        }
      }
      database.Close();
    }
    

    project.setFeatureMatching(feature_matching_properties);
    writeMatchPairs(&project);
    project.save(project_file);

  } catch (const std::exception &e) {

    printException(e);

    r = true;
  }

  return r;
}

void FeatureMatchingCommand::writeMatchPairs(Project *project)
{
  QString database_file = project->database();
  colmap::Database database(database_file.toStdString());
  std::vector<colmap::Image> db_images = database.ReadAllImages();
  colmap::image_t image_id_l = 0;
  colmap::image_t image_id_r = 0;
  for(size_t i = 0; i < db_images.size(); i++) {
    image_id_l = db_images[i].ImageId();
    for(size_t j = 0; j < i; j++) {
      image_id_r = db_images[j].ImageId();

      colmap::FeatureMatches matches = database.ReadMatches(image_id_l, image_id_r);
      if(matches.size() > 0) {
        QString path_left = QFileInfo(db_images[i].Name().c_str()).baseName();
        QString path_right = QFileInfo(db_images[j].Name().c_str()).baseName();
        project->addMatchesPair(path_left, path_right);
      }
    }
  }
}

} // namespace graphos