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

#include "RelativeOrientationProcess.h"

//#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

#include <colmap/controllers/incremental_mapper.h>
#include <colmap/controllers/bundle_adjustment.h>
#include <colmap/util/misc.h>

namespace graphos
{

RelativeOrientationProcess::RelativeOrientationProcess(const QString &database,
                                                       const QString &outputPath,
                                                       bool fixCalibration)
  : tl::TaskBase(),
    mDatabase(database),
    mOutputPath(outputPath),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mBundleAdjustmentController(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager),
    mRefineFocalLength(!fixCalibration),
    mRefinePrincipalPoint(!fixCalibration),
    mRefineExtraParams(!fixCalibration)
{

}

RelativeOrientationProcess::~RelativeOrientationProcess()
{
  if(mIncrementalMapper) {
    delete mIncrementalMapper;
    mIncrementalMapper = nullptr;
  }

  if(mMapper) {
    delete mMapper;
    mMapper = nullptr;
  }

  if(mBundleAdjustmentController) {
    delete mBundleAdjustmentController;
    mBundleAdjustmentController = nullptr;
  }

  mReconstructionManager->Clear();
  mReconstructionManager.reset();
}

void RelativeOrientationProcess::stop()
{
  TaskBase::stop();
  
  if (mMapper && mMapper->IsRunning())
    mMapper->Stop();

  if (mBundleAdjustmentController && mBundleAdjustmentController->IsRunning())
    mBundleAdjustmentController->Stop();
}

void RelativeOrientationProcess::execute(tl::Progress *progressBar)
{
  try {

    msgInfo("Starting Orientation");

    tl::Chrono chrono("Orientation process finished");
    chrono.run();

    mReconstructionManager->Clear();

    std::string sparse_path = mOutputPath.toStdString();

    tl::Path dir(sparse_path);
    if(!dir.exists() && !dir.createDirectories()) {
      throw std::runtime_error(std::string("Directory couldn't be created: ").append(sparse_path));
    }

    if(mMapper) {
      delete mMapper;
      mMapper = nullptr;
    }

    mIncrementalMapper->ba_refine_focal_length = mRefineFocalLength;
    mIncrementalMapper->ba_refine_principal_point = false;
    mIncrementalMapper->ba_refine_extra_params = mRefineExtraParams;
    //mIncrementalMapper->min_num_matches = 30;
    //mIncrementalMapper->ba_global_images_ratio = 1.2;
    //mIncrementalMapper->ba_global_points_ratio = 1.2;
    //mIncrementalMapper->ba_global_max_num_iterations = 20;
    //mIncrementalMapper->ba_global_max_refinements = 3;
    //mIncrementalMapper->ba_global_points_freq = 200000;

    mMapper = new colmap::IncrementalMapperController(mIncrementalMapper,
                                                      "",
                                                      mDatabase.toStdString(),
                                                      mReconstructionManager.get());

    size_t prev_num_reconstructions = 0;
    mMapper->AddCallback(
      colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {
          try {
            // If the number of reconstructions has not changed, the last model
            // was discarded for some reason.
            if(mReconstructionManager->Size() > prev_num_reconstructions) {
              const std::string reconstruction_path = sparse_path;
              const auto &reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
              colmap::CreateDirIfNotExists(reconstruction_path);
              reconstruction.Write(reconstruction_path);
              //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
              ///TODO: Por ahora sólo trabajamos con una reconstrucción
              //prev_num_reconstructions = mReconstructionManager->Size();
            }
          } catch(std::exception &e) {
            msgError(e.what());
          }
      });

    mMapper->AddCallback(
      colmap::IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [&]() {
          //if (progressBar) (*progressBar)();
          //msgInfo("-----");
      });

    mMapper->Start(); ///TODO: ¿Como detectar que se ha producido un error?
    mMapper->Wait();

    if(status() == tl::Task::Status::stopping) return;

    if(mReconstructionManager->Size() == 0) throw std::runtime_error("Reconstruction fail");

    colmap::OptionManager optionManager;
    optionManager.bundle_adjustment->refine_focal_length = mRefineFocalLength;
    optionManager.bundle_adjustment->refine_principal_point = mRefinePrincipalPoint;
    optionManager.bundle_adjustment->refine_extra_params = mRefineExtraParams;


    //for (size_t id = 0; id < mReconstructionManager->Size(); id++) {
    //  colmap::Reconstruction& reconstruction = mReconstructionManager->Get(id);
    colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);

    mBundleAdjustmentController = new colmap::BundleAdjustmentController(optionManager, &reconstruction);

    mBundleAdjustmentController->Start();
    mBundleAdjustmentController->Wait();

    if(status() == tl::Task::Status::stopping) return;

    OrientationExport orientationExport(&reconstruction);

    QString path = QString::fromStdString(sparse_path);
    orientationExport.exportBinary(path);
    orientationExport.exportPLY(path + "/sparse.ply");

    if(status() == tl::Task::Status::stopping) return;

    chrono.stop();
    
    emit orientation_finished();

    if(progressBar) (*progressBar)();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Relative Orientation error");
  }
}

} // namespace graphos
