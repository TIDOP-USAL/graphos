#include "DtmProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>
#include <tidop/geometry/size.h>

#include "inspector/core/densification/DenseExport.h"

#include <QFileInfo>
#include <QDir>

namespace inspector
{

DtmProcess::DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
                       const std::shared_ptr<DenseExport> &denseExport,
                       const QString &dtmFile,
                       double gsd,
                       bool dsm)
  : ProcessConcurrent(),
    mDtmAlgorithm(dtmAlgorithm),
    mDenseExport(denseExport),
    mDtmFile(dtmFile),
    mGSD(gsd),
    mDSM(dsm)
{

}

void DtmProcess::run()
{
  try {

    msgInfo("Start DTM generation");

    tl::Chrono chrono("DTM finished");
    chrono.run();

    if (mDSM) {

      /// Aplicar filtro CSF


    } else {

      QFileInfo info(mDtmFile);

      QDir dir = info.absoluteDir();
      if (!dir.exists()) {
        if (dir.mkpath(".") == false) {
          throw std::runtime_error("The output directory cannot be created");
        }
      }

      QString point_cloud = info.path() + "/" + info.completeBaseName() + ".csv";

      tl::BoundingBox<tl::Point3<double>> bbox;

      mDenseExport->exportToCSV(point_cloud, DenseExport::Fields::rgb, &bbox);

      tl::Size<int> size(bbox.width() / mGSD, bbox.height() / mGSD);

      mDtmAlgorithm->run(point_cloud, mDtmFile, size);
    }
    
    chrono.stop();

    emit dtmFinished();

  } catch (std::exception &e) {
    emit error(0, "DTM error");
    msgError(e.what());
  }
}


} // inspector
