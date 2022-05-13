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
    mProject->projectFolder();
    QString output_path = mProject->projectFolder() + "/dense";

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

      densifier = std::make_shared<CmvsPmvsDensifier>(mPmvsUseVisibilityInformation,
                                                      mPmvsImagesPerCluster,
                                                      mPmvsLevel,
                                                      mPmvsCellSize,
                                                      mPmvsThreshold,
                                                      mPmvsWindowSize,
                                                      mPmvsMinimunImageNumber,
                                                      !mDisableCuda);
    } else if (mDensificationMethod == "SMVS") {

      msgInfo("SMVS Properties:");
      msgInfo("- Input Image Scale: %i", mSmvsInputImageScale);
      msgInfo("- Output Depth Scale: %i", mSmvsOutputDepthScale);
      msgInfo("- Shading Based Optimization: %s", mSmvsShadingBasedOptimization ? "true" : "false");
      msgInfo("- Semi-global Matching: %s", mSmvsSemiGlobalMatching ? "true" : "false");
      msgInfo("- Surface Smoothing Factor: %lf", mSmvsSurfaceSmoothingFactor);

      densifier = std::make_shared<SmvsDensifier>(mSmvsInputImageScale,
                                                  mSmvsOutputDepthScale,
                                                  mSmvsShadingBasedOptimization,
                                                  mSmvsSemiGlobalMatching,
                                                  mSmvsSurfaceSmoothingFactor,
                                                  !mDisableCuda);
    
    } else {
      throw std::runtime_error("Densification Method not valid");
    }

    mProject->setDensification(std::dynamic_pointer_cast<Densification>(densifier));

    densifier->undistort(mProject->reconstructionPath(), output_path);
    densifier->densify(output_path);

    QString dense_model = output_path;
    if (mDensificationMethod == "PMVS") {
      dense_model.append("/pmvs/models/option-all.ply");
    } else {
      dense_model.append("/smvs-");
      if (mSmvsShadingBasedOptimization) 
        dense_model.append("S");
      else 
        dense_model.append("B");
      dense_model.append(QString::number(mSmvsInputImageScale)).append(".ply");
    }

    msgInfo("Dense model write at: %s", dense_model.toStdString().c_str());

    if (QFileInfo(dense_model).exists()) {
      mProject->setDenseModel(dense_model);
    } else {
      throw std::runtime_error("Densification failed");
    }

    mProject->save(project_file);

    chrono.stop();

  } catch (const std::exception &e) {

    printException(e);

    r = true;
  }

  return r;
}

} // namespace graphos