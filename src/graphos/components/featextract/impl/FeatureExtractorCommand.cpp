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


#include "FeatureExtractorCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/features/featio.h"
#include "graphos/core/features/sift.h"
#include "graphos/core/project.h"
#include "graphos/components/featextract/impl/FeatureExtractorProcess.h"

#include <tidop/core/messages.h>

#include <colmap/base/database.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

#include <atomic>

using namespace tl;

namespace graphos
{

FeatureExtractorCommand::FeatureExtractorCommand()
  : Command("featextract", "Feature extraction (SIFT)"),
    mProjectFile(""),
    mMaxImageSize(3200),
    mDisableCuda(false)
{
  SiftProperties siftProperties;
  mMaxFeaturesNumber = siftProperties.featuresNumber();
  mOctaveResolution = siftProperties.octaveLayers();
  mContrastThreshold = siftProperties.contrastThreshold();
  mEdgeThreshold = siftProperties.edgeThreshold();

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentIntegerOptional("max_image_size", 's', std::string("Maximum image size (default = ").append(std::to_string(mMaxImageSize)).append(")"), &mMaxImageSize));
  this->push_back(CreateArgumentIntegerOptional("max_features_number", std::string("Maximum number of features to detect (default = ").append(std::to_string(mMaxFeaturesNumber)).append(")"), &mMaxFeaturesNumber));
  this->push_back(CreateArgumentIntegerOptional("SIFT:octave_resolution", std::string("SIFT: Number of layers in each octave (default = ").append(std::to_string(mOctaveResolution)).append(")"), &mOctaveResolution));
  this->push_back(CreateArgumentDoubleOptional("SIFT:contrast_threshold", std::string("SIFT: Contrast Threshold (default = ").append(std::to_string(mContrastThreshold)).append(")"), &mContrastThreshold));
  this->push_back(CreateArgumentDoubleOptional("SIFT:edge_threshold", std::string("SIFT: Threshold used to filter out edge-like features (default = ").append(std::to_string(mEdgeThreshold)).append(")"), &mEdgeThreshold));
  
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

  this->addExample("featextract --p 253/253.xml");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureExtractorCommand::~FeatureExtractorCommand()
{
}

bool FeatureExtractorCommand::run()
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
    QString database_path = project.database();
    std::vector<Image> images = project.images();
    std::map<int, Camera> cameras = project.cameras();

    TL_TODO("Colmap no permite borrar la tabla de keypoints ni sobreescribirla asi que por ahora borro la base de datos completa")
    QFile(database_path).remove();
    project.removeFeatures();

    colmap::Database database(database_path.toStdString());

    std::shared_ptr<FeatureExtractor> feature_extractor;

    if(mDisableCuda) {
      feature_extractor = std::make_shared<SiftCPUDetectorDescriptor>(mMaxFeaturesNumber,
                                                                      mOctaveResolution,
                                                                      mEdgeThreshold,
                                                                      mContrastThreshold);
      
    } else {
      feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mMaxFeaturesNumber,
                                                                       mOctaveResolution,
                                                                       mEdgeThreshold,
                                                                       mContrastThreshold);
    }

    FeatureExtractorProcess Feature_extractor_process(images,
                                                      cameras,
                                                      database_path,
                                                      mMaxImageSize,
                                                      !mDisableCuda,
                                                      feature_extractor);

    connect(&Feature_extractor_process, &FeatureExtractorProcess::featuresExtracted, 
            [&](const QString &imageName, const QString &featuresFile){
              project.addFeatures(imageName, featuresFile);
            });

    Feature_extractor_process.run();

    project.setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feature_extractor));
    project.save(project_file);

  } catch (const std::exception &e) {

    printException(e);
    
    r = true;
  }

  return r;
}

} // namespace graphos