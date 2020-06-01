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

OrientationColmapProperties::OrientationColmapProperties()
  : mRefineFocalLength(true),
    mRefinePrincipalPoint(false),
    mRefineExtraParams(true)
{

}

bool OrientationColmapProperties::refineFocalLength() const
{
  return mRefineFocalLength;
}

void OrientationColmapProperties::setRefineFocalLength(bool refineFocalLength)
{
  mRefineFocalLength = refineFocalLength;
}

bool OrientationColmapProperties::refinePrincipalPoint() const
{
  return mRefinePrincipalPoint;
}

void OrientationColmapProperties::setRefinePrincipalPoint(bool refinePrincipalPoint)
{
  mRefinePrincipalPoint = refinePrincipalPoint;
}

bool OrientationColmapProperties::refineExtraParams() const
{
  return mRefineExtraParams;
}

void OrientationColmapProperties::setRefineExtraParams(bool refineExtraParams)
{
  mRefineExtraParams = refineExtraParams;
}

void OrientationColmapProperties::reset()
{
  mRefineFocalLength = true;
  mRefinePrincipalPoint = false;
  mRefineExtraParams = true;
}

QString OrientationColmapProperties::name() const
{
  return QString("Colmap");
}



/*----------------------------------------------------------------*/



OrientationColmapProcess::OrientationColmapProcess(const QString &database,
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

OrientationColmapProcess::~OrientationColmapProcess()
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

void OrientationColmapProcess::run()
{
  try {

    mReconstructionManager->Clear();

    QString sparse_path = mOutputPath + "/sparse/";

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
            const std::string reconstruction_path = colmap::JoinPaths(sparse_path.toStdString(), std::to_string(prev_num_reconstructions));
            const auto& reconstruction = mReconstructionManager->Get(prev_num_reconstructions);
            colmap::CreateDirIfNotExists(reconstruction_path);
            reconstruction.Write(reconstruction_path);
            //mOptions->Write(JoinPaths(reconstruction_path, "project.ini"));
            prev_num_reconstructions = mReconstructionManager->Size();
          }
        } catch (std::exception &e) {
          msgError(e.what());
        }
    });

    mMapper->AddCallback(
      colmap::IncrementalMapperController::NEXT_IMAGE_REG_CALLBACK, [&]() {
      //if (progressBar) (*progressBar)();
      msgInfo("-----");
    });

    mMapper->Start(); ///TODO: ¿Como detectar que se ha producido un error?
    mMapper->Wait();

    if (mReconstructionManager->Size() == 0) {
      //throw  "Reconstruction fail";
      msgError("Reconstruction fail");
      return;
    }

    colmap::OptionManager optionManager;
    optionManager.bundle_adjustment->refine_focal_length = OrientationColmapProperties::refineFocalLength();
    optionManager.bundle_adjustment->refine_principal_point = OrientationColmapProperties::refinePrincipalPoint();
    optionManager.bundle_adjustment->refine_extra_params = OrientationColmapProperties::refineExtraParams();


    for (size_t id = 0; id < mReconstructionManager->Size(); id++) {
      colmap::Reconstruction& reconstruction = mReconstructionManager->Get(id);

      colmap::BundleAdjustmentController ba_controller(optionManager, &reconstruction);
      ba_controller.Start();
      ba_controller.Wait();

      OrientationExport orientationExport(mImagePath, &reconstruction);

      QString path = sparse_path + QString::number(id);

      orientationExport.exportBinary(path);
      orientationExport.exportPLY(path + "/sparse.ply");

#ifdef _DEBUG
      orientationExport.exportText(path);
#endif
    }

  } catch (std::exception &e) {
    msgError(e.what());
  } catch (...) {
    msgError("excepción desconocida");
  }
}

//void OrientationColmapProcess::setRefineFocalLength(bool refineFocalLength)
//{

//}

//void OrientationColmapProcess::setRefinePrincipalPoint(bool refinePrincipalPoint)
//{
//}

//void OrientationColmapProcess::setRefineExtraParams(bool refineExtraParams)
//{
//}

} // namespace inspector
