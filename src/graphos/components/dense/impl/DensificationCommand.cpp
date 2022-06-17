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


#include "DensificationCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/Smvs.h"
#include "graphos/core/dense/mvs.h"

#include <tidop/core/chrono.h>

#include <QFileInfo>

using namespace tl;

namespace graphos
{


DensificationCommand::DensificationCommand()
  : Command("dense", "Create a dense point cloud"),
    mProjectFile(""),
    mDensificationMethod(""),
    mPmvsUseVisibilityInformation(true),
    mPmvsImagesPerCluster(100),
    mPmvsLevel(1),
    mPmvsCellSize(2),
    mPmvsThreshold(0.7),
    mPmvsWindowSize(7),
    mPmvsMinimunImageNumber(3),
    mPmvsImageOriginalDepth(false),
    mSmvsInputImageScale(1),
    mSmvsOutputDepthScale(2),
    mSmvsShadingBasedOptimization(false),
    mSmvsSemiGlobalMatching(true),
    mSmvsSurfaceSmoothingFactor(1.0),
    mMvsResolutionLevel(1),
    mMvsMinResolution(640),
    mMvsMaxResolution(3200),
    mMvsNumberViews(5),
    mMvsNumberViewsFuse(3),
    mDisableCuda(false),
    mProject(nullptr)
{

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentStringRequired("method", 'm', "Densification Method (PMVS, SMVS)", &mDensificationMethod));
  this->push_back(CreateArgumentBooleanOptional("pmvs:visibility", std::string("Use Visibility Information (default=").append(mPmvsUseVisibilityInformation ? "true)" : "false)"), &mPmvsUseVisibilityInformation));
  this->push_back(CreateArgumentIntegerOptional("pmvs:images_per_cluster", std::string("Images per cluster (default=").append(std::to_string(mPmvsImagesPerCluster)).append(")"), &mPmvsImagesPerCluster));
  this->push_back(CreateArgumentIntegerOptional("pmvs:level", std::string("Level (default=").append(std::to_string(mPmvsLevel)).append(")"), &mPmvsLevel));
  this->push_back(CreateArgumentIntegerOptional("pmvs:cell_size", std::string("Cell Size (default=").append(std::to_string(mPmvsCellSize)).append(")"), &mPmvsCellSize));
  this->push_back(CreateArgumentDoubleOptional("pmvs:threshold", std::string("Threshold (default=").append(std::to_string(mPmvsThreshold)).append(")"), &mPmvsThreshold));
  this->push_back(CreateArgumentIntegerOptional("pmvs:window_size", std::string("Window Size (default=").append(std::to_string(mPmvsWindowSize)).append(")"), &mPmvsWindowSize));
  this->push_back(CreateArgumentIntegerOptional("pmvs:minimun_images", std::string("Minimun Image Number (default=").append(std::to_string(mPmvsMinimunImageNumber)).append(")"), &mPmvsMinimunImageNumber));
  this->push_back(CreateArgumentBooleanOptional("pmvs:original_depth", std::string("Image Original Depth (default=").append(mPmvsImageOriginalDepth ? "true)" : "false)"), &mPmvsImageOriginalDepth));
  this->push_back(CreateArgumentIntegerOptional("smvs:input_image_scale", std::string("Input Image Scale (default=").append(std::to_string(mSmvsInputImageScale)).append(")"), &mSmvsInputImageScale));
  this->push_back(CreateArgumentIntegerOptional("smvs:output_depth_scale", std::string("Output Depth Scale (default=").append(std::to_string(mSmvsOutputDepthScale)).append(")"), &mSmvsOutputDepthScale));
  this->push_back(CreateArgumentBooleanOptional("smvs:shading_optimization", std::string("Shading Based Optimization (default=").append(mSmvsShadingBasedOptimization ? "true)" : "false)"), &mSmvsShadingBasedOptimization));
  this->push_back(CreateArgumentBooleanOptional("smvs:sgm", std::string("Semi-global Matching (default=").append(mSmvsSemiGlobalMatching ? "true)" : "false)"), &mSmvsSemiGlobalMatching));
  this->push_back(CreateArgumentDoubleOptional("smvs:smooth_factor", std::string("Surface Smoothing Factor (default=").append(std::to_string(mSmvsSurfaceSmoothingFactor)).append(")"), &mSmvsSurfaceSmoothingFactor));
  this->push_back(CreateArgumentIntegerOptional("mvs:resolution_level", std::string("Resolution Level (default=").append(std::to_string(mMvsResolutionLevel)).append(")"), &mMvsResolutionLevel));
  this->push_back(CreateArgumentIntegerOptional("mvs:min_resolution", std::string("Min Resolution (default=").append(std::to_string(mMvsMinResolution)).append(")"), &mMvsMinResolution));
  this->push_back(CreateArgumentIntegerOptional("mvs:max_resolution", std::string("Max Resolution (default=").append(std::to_string(mMvsMaxResolution)).append(")"), &mMvsMaxResolution));
  this->push_back(CreateArgumentIntegerOptional("mvs:number_views", std::string("Number Views (default=").append(std::to_string(mMvsNumberViews)).append(")"), &mMvsNumberViews));
  this->push_back(CreateArgumentIntegerOptional("mvs:number_views_fuse", std::string("Number Views Fuse (default=").append(std::to_string(mMvsNumberViewsFuse)).append(")"), &mMvsNumberViewsFuse));

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

  this->addExample("dense --p 253/253.xml --method PMVS");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DensificationCommand::~DensificationCommand()
{
  if (mProject) {
    delete mProject;
    mProject = nullptr;
  }
}

bool DensificationCommand::run()
{
  bool r = false;

  QString file_path;
  QString project_path;

  try {

    tl::Chrono chrono("Densification finished");
    chrono.run();

    TL_ASSERT(mProjectFile.exists(), "Project doesn't exist");
    TL_ASSERT(mProjectFile.isFile(), "Project file doesn't exist");

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    mProject = new ProjectImp;
    mProject->load(project_file);

    tl::Path dense_path(mProject->projectFolder().toStdWString());
    dense_path.append("dense");

    tl::Path ground_points_path(mProject->reconstructionPath().toStdWString());
    ground_points_path.append("ground_points.bin");

    std::unique_ptr<GroundPointsReader> reader = GroundPointsReaderFactory::create("GRAPHOS");
    reader->read(ground_points_path);

    std::vector<GroundPoint> ground_points = reader->points();


    std::shared_ptr<Densifier> densifier;

    if (mDensificationMethod == "PMVS") {

      msgInfo("PMVS Properties:");
      msgInfo("- Use Visibility Information: %s", mPmvsUseVisibilityInformation ? "True" : "False");
      msgInfo("- Images per cluster: %i", mPmvsImagesPerCluster);
      msgInfo("- Level: %i", mPmvsLevel);
      msgInfo("- Cell Size: %i", mPmvsCellSize);
      msgInfo("- Threshold: %lf", mPmvsThreshold);
      msgInfo("- Window Size: %i", mPmvsWindowSize);
      msgInfo("- Minimun Image Number: %i", mPmvsMinimunImageNumber);
      msgInfo("- Image Original Depth: %s", mPmvsImageOriginalDepth ? "true" : "false");

      dense_path.append("pmvs");

      auto pmvs = std::make_shared<CmvsPmvsDensifier>(mProject->images(),
                                                      mProject->cameras(),
                                                      mProject->poses(),
                                                      ground_points,
                                                      QString::fromStdWString(dense_path.toWString()),
                                                      mProject->database(),
                                                      !mDisableCuda);

      pmvs->setUseVisibilityInformation(mPmvsUseVisibilityInformation);
      pmvs->setImagesPerCluster(mPmvsImagesPerCluster);
      pmvs->setLevel(mPmvsLevel);
      pmvs->setCellSize(mPmvsCellSize);
      pmvs->setThreshold(mPmvsThreshold);
      pmvs->setWindowSize(mPmvsWindowSize);
      pmvs->setMinimunImageNumber(mPmvsMinimunImageNumber);

      densifier = std::move(pmvs);

    } else if (mDensificationMethod == "SMVS") {

      msgInfo("SMVS Properties:");
      msgInfo("- Input Image Scale: %i", mSmvsInputImageScale);
      msgInfo("- Output Depth Scale: %i", mSmvsOutputDepthScale);
      msgInfo("- Shading Based Optimization: %s", mSmvsShadingBasedOptimization ? "true" : "false");
      msgInfo("- Semi-global Matching: %s", mSmvsSemiGlobalMatching ? "true" : "false");
      msgInfo("- Surface Smoothing Factor: %lf", mSmvsSurfaceSmoothingFactor);

      dense_path.append("smvs");

      auto smvs = std::make_shared<SmvsDensifier>(mProject->images(),
                                                  mProject->cameras(),
                                                  mProject->poses(),
                                                  ground_points,
                                                  QString::fromStdWString(dense_path.toWString()),
                                                  !mDisableCuda);

      smvs->setInputImageScale(mSmvsInputImageScale);
      smvs->setOutputDepthScale(mSmvsOutputDepthScale);
      smvs->setShadingBasedOptimization(mSmvsShadingBasedOptimization);
      smvs->setSemiGlobalMatching(mSmvsSemiGlobalMatching);
      smvs->setSurfaceSmoothingFactor(mSmvsSurfaceSmoothingFactor);

      densifier = smvs;

    } else if (mDensificationMethod == "MVS") {
    
      msgInfo("MVS Properties:");
      msgInfo("- Resolution Level: %i", mMvsResolutionLevel);
      msgInfo("- Min Resolution: %i", mMvsMinResolution);
      msgInfo("- Max Resolution: %i", mMvsMaxResolution);
      msgInfo("- Number Views: %i", mMvsNumberViews);
      msgInfo("- Number Views Fuse: %i", mMvsNumberViewsFuse);

      dense_path.append("mvs");

      auto mvs = std::make_shared<MvsDensifier>(mProject->images(),
                                                mProject->cameras(),
                                                mProject->poses(),
                                                ground_points,
                                                QString::fromStdWString(dense_path.toWString()),
                                                mProject->database(),
                                                !mDisableCuda);

      mvs->setMaxResolution(mMvsMaxResolution);
      mvs->setMinResolution(mMvsMinResolution);
      mvs->setNumberViews(mMvsNumberViews);
      mvs->setNumberViewsFuse(mMvsNumberViews);
      mvs->setResolutionLevel(mMvsResolutionLevel);

      densifier = mvs;

    } else {
      throw std::runtime_error("Densification Method not valid");
    }

    mProject->setDensification(std::dynamic_pointer_cast<Densification>(densifier));
    mProject->setDenseModel(densifier->denseModel());
    mProject->save(project_file);

    chrono.stop();

  } catch (const std::exception &e) {

    printException(e);

    r = true;
  }

  return r;
}

} // namespace graphos