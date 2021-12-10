/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "FeatureMatchingCommand.h"

#include "graphos/core/features/matching.h"
#include "graphos/core/project.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

#include <colmap/feature/matching.h>
#include <colmap/base/database.h>

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
  this->push_back(CreateArgumentDoubleOptional("ratio", 'r', std::string("Ratio (default = ").append(std::to_string(mRatio)), &mRatio));
  this->push_back(CreateArgumentDoubleOptional("distance", 'd', std::string("Distance (default = ").append(std::to_string(mDistance)), &mDistance));
  this->push_back(CreateArgumentDoubleOptional("max_error", 'e', std::string("Maximun error (default = ").append(std::to_string(mMaxError)), &mMaxError));
  this->push_back(CreateArgumentDoubleOptional("confidence", 'c', std::string("Confidence (default = ").append(std::to_string(mConfidence)), &mConfidence));
  this->push_back(CreateArgumentBooleanOptional("cross_check", 'x', std::string("If true, cross checking is enabled (default = ").append(mCrossCheck ? "true)" : "false)"), &mCrossCheck));
  this->push_back(CreateArgumentBooleanOptional("disable_cuda", "If true disable CUDA (default = false)", &mDisableCuda));
  this->push_back(CreateArgumentBooleanOptional("exhaustive_matching", "Force exhaustive matching (default = false)", &mForceExhaustiveMatching));
 
  this->addExample("featmatch --p 253/253.xml");

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

    tl::Chrono chrono("Feature Matching finished ");
    chrono.run();

    if (!mProjectFile.exists()) {
      msgError("Project doesn't exist");
      return 1;
    }

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    ProjectImp project;
    project.load(project_file);
    std::string database_path = project.database().toStdString();

    bool spatial_matching = false;

    auto it = project.imageBegin();
    if (it != project.imageEnd()) {
      CameraPose cameraPosition = it->cameraPose();
      if (!cameraPosition.isEmpty())
        spatial_matching = true;
    }
    
    std::shared_ptr<colmap::Thread> feature_matcher;

    colmap::SiftMatchingOptions sift_matching_options;
    sift_matching_options.max_error = mMaxError;
    sift_matching_options.cross_check = mCrossCheck;
    sift_matching_options.max_ratio = mRatio;
    sift_matching_options.max_distance = mDistance;
    sift_matching_options.confidence = mConfidence;
    sift_matching_options.use_gpu = !mDisableCuda;
    sift_matching_options.min_num_inliers = 15;// 100;


    if (spatial_matching && !mForceExhaustiveMatching) {

      colmap::SpatialMatchingOptions spatial_matching_options;
      spatial_matching_options.max_num_neighbors = 100;// 500;
      //spatialMatchingOptions.max_distance = 250;
      spatial_matching_options.ignore_z = true;
      spatial_matching_options.is_gps = false;

      feature_matcher = std::make_unique<colmap::SpatialFeatureMatcher>(spatial_matching_options,
                                                                        sift_matching_options,
                                                                        database_path);
      msgInfo("Spatial Matching");

    } else {

      colmap::ExhaustiveMatchingOptions exhaustive_matching_options;
      //exhaustive_matching_options.block_size = mFeatureMatching->blockSize();
      feature_matcher = std::make_unique<colmap::ExhaustiveFeatureMatcher>(exhaustive_matching_options,
                                                                           sift_matching_options,
                                                                           database_path);
      msgInfo("Exhaustive Matching");

    }

    feature_matcher->Start();
    feature_matcher->Wait();

    colmap::Database database(database_path);
    if (database.NumMatches() > 0) {

      std::shared_ptr<FeatureMatchingProperties> feature_matching_properties = std::make_shared<FeatureMatchingProperties>();
      feature_matching_properties->setRatio(mRatio);
      feature_matching_properties->setDistance(mDistance);
      feature_matching_properties->setMaxError(mMaxError);
      feature_matching_properties->setConfidence(mConfidence);
      feature_matching_properties->enableCrossCheck(mCrossCheck);
      project.setFeatureMatching(feature_matching_properties);

      project.save(project_file);

    } else {
      msgError("Feature Matching error: No matching points detected.");
    }

    database.Close();

    chrono.stop();

  } catch (const std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
    r = true;
  }

  return r;
}

} // namespace graphos