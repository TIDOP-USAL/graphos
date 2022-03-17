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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/path.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/controllers/hierarchical_mapper.h>
#include <colmap/controllers/bundle_adjustment.h>

namespace graphos
{

RelativeOrientationColmapProperties::RelativeOrientationColmapProperties()
  : mRefineFocalLength(true),
    mRefinePrincipalPoint(false),
    mRefineExtraParams(true)
{

}

bool RelativeOrientationColmapProperties::refineFocalLength() const
{
  return mRefineFocalLength;
}

void RelativeOrientationColmapProperties::setRefineFocalLength(bool refineFocalLength)
{
  mRefineFocalLength = refineFocalLength;
}

bool RelativeOrientationColmapProperties::refinePrincipalPoint() const
{
  return mRefinePrincipalPoint;
}

void RelativeOrientationColmapProperties::setRefinePrincipalPoint(bool refinePrincipalPoint)
{
  mRefinePrincipalPoint = refinePrincipalPoint;
}

bool RelativeOrientationColmapProperties::refineExtraParams() const
{
  return mRefineExtraParams;
}

void RelativeOrientationColmapProperties::setRefineExtraParams(bool refineExtraParams)
{
  mRefineExtraParams = refineExtraParams;
}

void RelativeOrientationColmapProperties::reset()
{
  mRefineFocalLength = true;
  mRefinePrincipalPoint = false;
  mRefineExtraParams = true;
}

QString RelativeOrientationColmapProperties::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



RelativeOrientationColmapAlgorithm::RelativeOrientationColmapAlgorithm(const QString &database,
                                                                       const QString &outputPath,
                                                                       bool fixCalibration)
  : mDatabase(database),
    mOutputPath(outputPath),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager)
{
  RelativeOrientationColmapProperties::setRefineFocalLength(!fixCalibration);
  RelativeOrientationColmapProperties::setRefinePrincipalPoint(!fixCalibration);
  RelativeOrientationColmapProperties::setRefineExtraParams(!fixCalibration);
}

RelativeOrientationColmapAlgorithm::~RelativeOrientationColmapAlgorithm()
{
  if (mIncrementalMapper) {
    delete mIncrementalMapper;
    mIncrementalMapper = nullptr;
  }

  if (mMapper) {
    delete mMapper;
    mMapper = nullptr;
  }

  mReconstructionManager->Clear();
  mReconstructionManager.reset();
}

void RelativeOrientationColmapAlgorithm::run()
{
  mReconstructionManager->Clear();

  std::string sparse_path = mOutputPath.toStdString();

  tl::Path dir(sparse_path);
  if (!dir.exists() && !dir.createDirectories()) {
    throw std::runtime_error(std::string("Directory couldn't be created: ").append(sparse_path));
  }

  if (mMapper) {
    delete mMapper;
    mMapper = nullptr;
  }
  
  mIncrementalMapper->ba_refine_focal_length = RelativeOrientationColmapProperties::refineFocalLength();
  mIncrementalMapper->ba_refine_principal_point = false;
  mIncrementalMapper->ba_refine_extra_params = RelativeOrientationColmapProperties::refineExtraParams();
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
        if (mReconstructionManager->Size() > prev_num_reconstructions) {
          const std::string reconstruction_path = sparse_path;
          const auto &reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
          colmap::CreateDirIfNotExists(reconstruction_path);
          reconstruction.Write(reconstruction_path);
          //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
          ///TODO: Por ahora sólo trabajamos con una reconstrucción
          //prev_num_reconstructions = mReconstructionManager->Size();
        }
      } catch (std::exception &e) {
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
  
  if (mReconstructionManager->Size() == 0) throw std::runtime_error("Reconstruction fail");
  
  colmap::OptionManager optionManager;
  optionManager.bundle_adjustment->refine_focal_length = RelativeOrientationColmapProperties::refineFocalLength();
  optionManager.bundle_adjustment->refine_principal_point = RelativeOrientationColmapProperties::refinePrincipalPoint();
  optionManager.bundle_adjustment->refine_extra_params = RelativeOrientationColmapProperties::refineExtraParams();
  
  
  //for (size_t id = 0; id < mReconstructionManager->Size(); id++) {
  //  colmap::Reconstruction& reconstruction = mReconstructionManager->Get(id);
  colmap::Reconstruction &reconstruction = mReconstructionManager->Get(0);
  
  colmap::BundleAdjustmentController ba_controller(optionManager, &reconstruction);
  ba_controller.Start();
  ba_controller.Wait();
  
  OrientationExport orientationExport(&reconstruction);
  
  QString path = QString::fromStdString(sparse_path);
  orientationExport.exportBinary(path);
  orientationExport.exportPLY(path + "/sparse.ply");

}



/*----------------------------------------------------------------*/

constexpr auto min_common_images = 3;
constexpr auto robust_alignment = true;
constexpr auto robust_alignment_max_error = 1.;

AbsoluteOrientationColmapProperties::AbsoluteOrientationColmapProperties()
  : mMinCommonImages(min_common_images),
    mRobustAlignment(robust_alignment),
    mRobustAlignmentMaxError(robust_alignment_max_error)
{
}

int AbsoluteOrientationColmapProperties::minCommonImages() const
{
  return mMinCommonImages;
}

void AbsoluteOrientationColmapProperties::setMinCommonImages(int minCommonImages)
{
  mMinCommonImages = minCommonImages;
}

bool AbsoluteOrientationColmapProperties::robustAlignment() const
{
  return mRobustAlignment;
}

void AbsoluteOrientationColmapProperties::setRobustAlignment(bool robustAlignment)
{
  mRobustAlignment = robustAlignment;
}

double AbsoluteOrientationColmapProperties::robustAlignmentMaxError() const
{
  return mRobustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::setRobustAlignmentMaxError(double robustAlignmentMaxError)
{
  mRobustAlignmentMaxError = robustAlignmentMaxError;
}

void AbsoluteOrientationColmapProperties::reset()
{
  mMinCommonImages = min_common_images;
  mRobustAlignment = robust_alignment;
  mRobustAlignmentMaxError = robust_alignment_max_error;
}

QString AbsoluteOrientationColmapProperties::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



AbsoluteOrientationColmapAlgorithm::AbsoluteOrientationColmapAlgorithm(const QString &inputPath,
                                                                       const std::map<QString, std::array<double, 3>>  &cameraPositions,
                                                                       const QString &outputPath)
  : mInputPath(inputPath),
    mCameraPositions(cameraPositions),
    mOutputPath(outputPath)
{
}

AbsoluteOrientationColmapAlgorithm::~AbsoluteOrientationColmapAlgorithm()
{
}

void AbsoluteOrientationColmapAlgorithm::run()
{
  bool robust_alignment = AbsoluteOrientationColmapProperties::robustAlignment();
  colmap::RANSACOptions ransac_options;
  ransac_options.max_error = AbsoluteOrientationColmapProperties::robustAlignmentMaxError();
  int min_common_images = AbsoluteOrientationColmapProperties::minCommonImages();

  tl::Path dir(mOutputPath.toStdString());
  if (!dir.exists() && !dir.createDirectories()) {
    throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toStdString()));
  }

  if (robust_alignment && ransac_options.max_error <= 0) {
    throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
  }

  std::vector<std::string> ref_image_names;
  std::vector<Eigen::Vector3d> ref_locations;
  //std::vector<std::string> lines = colmap::ReadTextFileLines(mImagePath.toStdString());
  //for (const auto line : lines) {
  //  std::stringstream line_parser(line);
  //  std::string image_name = "";
  //  Eigen::Vector3d camera_position;
  //  line_parser >> image_name >> camera_position[0] >> camera_position[1] >>
  //    camera_position[2];
  //  ref_image_names.push_back(image_name);
  //  ref_locations.push_back(camera_position);
  //}

  Eigen::Vector3d offset(0.,0.,0.);

  double i = 1.;
  for (const auto &cameraPosition : mCameraPositions) {
    std::string image_name = cameraPosition.first.toStdString();
    ref_image_names.push_back(image_name);
    Eigen::Vector3d camera_position;
    camera_position[0] = cameraPosition.second[0];
    camera_position[1] = cameraPosition.second[1];
    camera_position[2] = cameraPosition.second[2];

    //Para evitar desbordamiento
    offset += (camera_position - offset)/(i);
    ref_locations.push_back(camera_position);
    i++;
  }
  

  QString offset_file = mOutputPath + "/offset.txt";
  std::ofstream stream(offset_file.toStdString(), std::ios::trunc);
  if (stream.is_open()) {
    stream << QString::number(offset[0], 'f', 6).toStdString() << " "
           << QString::number(offset[1], 'f', 6).toStdString() << " "
           << QString::number(offset[2], 'f', 6).toStdString() << std::endl;

    msgInfo("Camera offset: %lf,%lf,%lf", offset[0], offset[1], offset[2] );

    stream.close();
  }

  for (auto &pos : ref_locations){
    pos -= offset;
  }

  if (!tl::Path(mInputPath.toStdString()).exists())
    throw std::runtime_error(std::string("Reconstruction not found in path: ").append(mInputPath.toStdString()));

  colmap::Reconstruction reconstruction;
  reconstruction.Read(mInputPath.toStdString());

  bool alignment_success;
  if (robust_alignment) {
    alignment_success = reconstruction.AlignRobust(
      ref_image_names, ref_locations, min_common_images, ransac_options);
  } else {
    alignment_success =
      reconstruction.Align(ref_image_names, ref_locations, min_common_images);
  }

  if (!alignment_success) throw std::runtime_error("Absolute Orientation failed");

  reconstruction.Write(mOutputPath.toStdString());

  OrientationExport orientationExport(&reconstruction);
  orientationExport.exportPLY(mOutputPath + "/sparse.ply");

  std::vector<double> errors;
  errors.reserve(ref_image_names.size());

  for (size_t i = 0; i < ref_image_names.size(); ++i) {
    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
    if (image != nullptr) {
      double error = (image->ProjectionCenter() - ref_locations[i]).norm();
      std::cout << image->Name() << "[ Error: " << error << std::endl;
      errors.push_back(error);
    }
  }

  msgInfo("Alignment error: %f (mean), %f (median)",
          colmap::Mean(errors), colmap::Median(errors));
}


/*----------------------------------------------------------------*/


AbsoluteOrientationColmapProperties2::AbsoluteOrientationColmapProperties2()
{
}


void AbsoluteOrientationColmapProperties2::reset()
{

}

QString AbsoluteOrientationColmapProperties2::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



AbsoluteOrientationColmapAlgorithm2::AbsoluteOrientationColmapAlgorithm2(const QString &inputPath,
                                                                       const std::map<QString, std::array<double, 7>>  &cameraPositions,
                                                                       const QString &outputPath)
  : mInputPath(inputPath),
    mCameraPositions(cameraPositions),
    mOutputPath(outputPath)
{
}

AbsoluteOrientationColmapAlgorithm2::~AbsoluteOrientationColmapAlgorithm2()
{
}

void AbsoluteOrientationColmapAlgorithm2::run()
{

}





void colmapRemoveOrientations(const std::vector<std::string> &images, const std::string &reconstruction)
{
  colmap::Reconstruction _reconstruction;
  _reconstruction.Read(reconstruction);

  for (const auto &image : images) {
    if (const colmap::Image *_image = _reconstruction.FindImageWithName(image)) {
      _reconstruction.DeRegisterImage(_image->ImageId());
    }
  }

  _reconstruction.Write(reconstruction);
}

} // namespace graphos
