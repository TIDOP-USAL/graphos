#include "orientationcolmap.h"

#include "inspector/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/controllers/hierarchical_mapper.h>
#include <colmap/controllers/bundle_adjustment.h>

#include <QDir>

namespace inspector
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
                                                   const QString &imagePath,
                                                   const QString &outputPath)
  : mDatabase(database),
    mImagePath(imagePath),
    mOutputPath(outputPath),
    mIncrementalMapper(new colmap::IncrementalMapperOptions),
    mMapper(nullptr),
    mReconstructionManager(new colmap::ReconstructionManager)
{

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
  try {

    mReconstructionManager->Clear();

    QString sparse_path = mOutputPath;// +"/sparse/";

    QDir dir(sparse_path);
    if (!dir.exists()){
      if (dir.mkpath(".") == false){
        QByteArray ba = sparse_path.toLocal8Bit();
        msgError("Directory cannot be created: %s", ba.data());
        return;
      }
    }

    if (mMapper) {
      delete mMapper;
      mMapper = nullptr;
    }
    mMapper = new colmap::IncrementalMapperController(mIncrementalMapper,
                                                      mImagePath.toStdString(),
                                                      mDatabase.toStdString(),
                                                      mReconstructionManager.get());

    size_t prev_num_reconstructions = 0;
    mMapper->AddCallback(
      colmap::IncrementalMapperController::LAST_IMAGE_REG_CALLBACK, [&]() {
        try {
          // If the number of reconstructions has not changed, the last model
          // was discarded for some reason.
          if (mReconstructionManager->Size() > prev_num_reconstructions) {
            const std::string reconstruction_path = sparse_path.toStdString(); //colmap::JoinPaths(sparse_path.toStdString(), std::to_string(prev_num_reconstructions));
            const auto& reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
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

    if (mReconstructionManager->Size() == 0) {
      //throw  "Reconstruction fail";
      msgError("Reconstruction fail");
      return;
    }

    colmap::OptionManager optionManager;
    optionManager.bundle_adjustment->refine_focal_length = RelativeOrientationColmapProperties::refineFocalLength();
    optionManager.bundle_adjustment->refine_principal_point = RelativeOrientationColmapProperties::refinePrincipalPoint();
    optionManager.bundle_adjustment->refine_extra_params = RelativeOrientationColmapProperties::refineExtraParams();


    //for (size_t id = 0; id < mReconstructionManager->Size(); id++) {
    //  colmap::Reconstruction& reconstruction = mReconstructionManager->Get(id);
      colmap::Reconstruction& reconstruction = mReconstructionManager->Get(0);

      colmap::BundleAdjustmentController ba_controller(optionManager, &reconstruction);
      ba_controller.Start();
      ba_controller.Wait();

      OrientationExport orientationExport(&reconstruction);

      QString path = sparse_path; // +QString::number(id);

      orientationExport.exportBinary(path);
      orientationExport.exportPLY(path + "/sparse.ply");

#ifdef _DEBUG
      orientationExport.exportText(path);
#endif
    //}

  } catch (std::exception &e) {
    msgError(e.what());
  } catch (...) {
    msgError("excepción desconocida");
  }
}



/*----------------------------------------------------------------*/

constexpr auto min_common_images = 3;
constexpr auto robust_alignment = false;
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

  Eigen::Vector3d center;

  for (const auto &cameraPosition : mCameraPositions) {
    std::string image_name = cameraPosition.first.toStdString();
    ref_image_names.push_back(image_name);
    Eigen::Vector3d camera_position;
    camera_position[0] = cameraPosition.second[0];
    camera_position[1] = cameraPosition.second[1];
    camera_position[2] = cameraPosition.second[2];
    center += camera_position;
    ref_locations.push_back(camera_position);
  }
  center /= mCameraPositions.size();
  msgInfo("Desplazamiento cámaras %lf,%lf,%lf", center[0], center[1], center[2] );

  QString offset = mOutputPath + "/offset.txt";
  std::ofstream stream(offset.toStdString(), std::ios::trunc);
  if (stream.is_open()) {
    stream << QString::number(center[0], 'f', 3).toStdString() << " "
           << QString::number(center[1], 'f', 3).toStdString() << " "
           << QString::number(center[2], 'f', 3).toStdString() << std::endl;
  }

  for (auto &pos : ref_locations){
    pos -= center;
  }

  colmap::Reconstruction reconstruction;
  reconstruction.Read(mInputPath.toStdString());

  msgInfo("Absolute Orientation");

  bool alignment_success;
  if (robust_alignment) {
    alignment_success = reconstruction.AlignRobust(
      ref_image_names, ref_locations, min_common_images, ransac_options);
  } else {
    alignment_success =
      reconstruction.Align(ref_image_names, ref_locations, min_common_images);
  }

  if (!alignment_success) throw std::runtime_error("Absolute Orientation failed");

  msgInfo("Absolute Orientation succeeded");
  colmap::CreateDirIfNotExists(mOutputPath.toStdString());
  reconstruction.Write(mOutputPath.toStdString());

  OrientationExport orientationExport(&reconstruction);
  orientationExport.exportPLY(mOutputPath + "/sparse.ply");

  std::vector<double> errors;
  errors.reserve(ref_image_names.size());

  for (size_t i = 0; i < ref_image_names.size(); ++i) {
    const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
    if (image != nullptr) {
      errors.push_back((image->ProjectionCenter() - ref_locations[i]).norm());
    }
  }

  msgInfo("Alignment error: %f (mean), %f (median)",
          colmap::Mean(errors), colmap::Median(errors));
}



} // namespace inspector
