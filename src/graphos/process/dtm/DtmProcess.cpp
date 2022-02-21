#include "DtmProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>
#include <tidop/geometry/size.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/imgwriter.h>
#include <tidop/geospatial/crs.h>

#include "graphos/core/densification/DenseExport.h"
#include "graphos/core/densification/DenseExport.h"
#include "graphos/core/dtm/csf.h"

namespace graphos
{

DtmProcess::DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
                       const QString &pointCloud,
                       const tl::Point3<double> &offset,
                       const QString &dtmFile,
                       double gsd,
                       bool dsm,
                       const QString &crs)
  : ProcessConcurrent(),
    mDtmAlgorithm(dtmAlgorithm),
    mPointCloud(pointCloud),
    mOffset(offset),
    mDtmFile(dtmFile),
    mGSD(gsd),
    mDSM(dsm),
    mCrs(crs)
{

}

void DtmProcess::run()
{
  try {

    msgInfo("Start DTM generation");

    tl::Chrono chrono("DTM finished");
    chrono.run();

    tl::Path dtm_path(mDtmFile.toStdString());
    tl::Path dtm_folder = dtm_path.parentPath();
    dtm_folder.createDirectories();

    //tl::TemporalDir temporal_dir;
    //tl::Path temp_path = temporal_dir.path();

    if (mDSM) {

      tl::Path ground_path(dtm_folder/*temp_path*/);
      ground_path.append("ground.ply");

      tl::Path out_ground_path(dtm_folder/*temp_path*/);
      out_ground_path.append("out_ground.ply");

      Csf csf;
      csf.setSloopSmooth(true);
      csf.filter(mPointCloud.toStdString(), ground_path.toString(), out_ground_path.toString());

      /// Refactorizar

      tl::Path mds_ground(dtm_folder/*temp_path*/);
      mds_ground.append("mds_ground.tif");
      tl::Path mds_out_ground(dtm_folder/*temp_path*/);
      mds_out_ground.append("mds_out_ground.tif");

      {
        tl::Path mds_ground_csv(dtm_folder/*temp_path*/);
        mds_ground_csv.append("mds_ground.csv");

        tl::BoundingBox<tl::Point3<double>> bbox_ground;

        DenseExport denseExport(ground_path.toString());
        denseExport.setOffset(mOffset);
        denseExport.exportToCSV(mds_ground_csv.toString(), DenseExport::Fields::xyz, &bbox_ground);

        tl::Path mds_out_ground_csv(dtm_folder/*temp_path*/);
        mds_out_ground_csv.append("mds_out_ground.csv");

        tl::BoundingBox<tl::Point3<double>> bbox_out_ground;

        DenseExport denseExport2(out_ground_path.toString());
        denseExport2.setOffset(mOffset);
        denseExport2.exportToCSV(mds_out_ground_csv.toString(), DenseExport::Fields::xyz, &bbox_out_ground);

        tl::BoundingBox<tl::Point3<double>> bbox = tl::joinBoundingBoxes(bbox_ground, bbox_out_ground);

        mDtmAlgorithm->run(mds_ground_csv.toString(), mds_ground.toString(), bbox, mGSD);
        mDtmAlgorithm->run(mds_out_ground_csv.toString(), mds_out_ground.toString(), bbox, mGSD);

      }

      /// Fusionar mds...
      
      std::unique_ptr<tl::ImageReader> image_reader_mds_ground = tl::ImageReaderFactory::createReader(mds_ground.toString());
      std::unique_ptr<tl::ImageReader> image_reader_mds_out_ground = tl::ImageReaderFactory::createReader(mds_out_ground.toString());
      std::unique_ptr<tl::ImageWriter> image_writer_mds_ground = tl::ImageWriterFactory::createWriter(mDtmFile.toStdString());

      image_reader_mds_ground->open();
      image_reader_mds_out_ground->open();

      if (image_reader_mds_ground->isOpen() && image_reader_mds_out_ground->isOpen()) {

        cv::Mat ground = image_reader_mds_ground->read();
        cv::Mat out_ground = image_reader_mds_out_ground->read();


        cv::Mat mask = cv::Mat::zeros(ground.size(), CV_8U);
        mask.setTo(1, out_ground > -9999);

        out_ground.copyTo(ground, mask);
        out_ground.release();
        mask.release();

        image_writer_mds_ground->open();
        if (image_writer_mds_ground->isOpen()) {
          tl::Affine<tl::PointD> georeference = image_reader_mds_ground->georeference();
          image_writer_mds_ground->create(image_reader_mds_ground->rows(), 
                                          image_reader_mds_ground->cols(), 
                                          image_reader_mds_ground->channels(), 
                                          image_reader_mds_ground->dataType());
          image_writer_mds_ground->setGeoreference(georeference);
          tl::geospatial::Crs crs(mCrs.toStdString());
          image_writer_mds_ground->setCRS(crs.toWktFormat());
          image_writer_mds_ground->setNoDataValue(-9999.);
          image_writer_mds_ground->write(ground);
          ground.release();

          image_writer_mds_ground->close();
        }

        image_reader_mds_ground->close();
        image_reader_mds_out_ground->close();

      }

    } else {

      tl::Path csv_path(dtm_folder/*temp_path*/);
      csv_path.append("dtm.csv");

      tl::BoundingBox<tl::Point3<double>> bbox;

      DenseExport denseExport(mPointCloud.toStdString());
      denseExport.setOffset(mOffset);
      denseExport.exportToCSV(csv_path.toString(), DenseExport::Fields::xyz, &bbox);

      tl::Size<int> size(bbox.width() / mGSD, bbox.height() / mGSD);

      mDtmAlgorithm->run(csv_path.toString(), mDtmFile.toStdString(), size);

    }
    
    chrono.stop();

    emit dtmFinished();

  } catch (std::exception &e) {
    emit error(0, "DTM error");
    msgError(e.what());
  }
}

} // graphos
