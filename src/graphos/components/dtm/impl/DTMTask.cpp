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

#include "DTMTask.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>
#include <tidop/geometry/size.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/imgwriter.h>
#include <tidop/geospatial/crs.h>

#include "graphos/core/dense/DenseExport.h"
#include "graphos/core/dense/DenseExport.h"
#include "graphos/core/dtm/csf.h"

namespace graphos
{

DtmProcess::DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
                       const tl::Path &pointCloud,
                       const tl::Point3<double> &offset,
                       const tl::Path &dtmFile,
                       double gsd,
                       bool dsm,
                       const QString &crs)
  : tl::TaskBase(),
    mDtmAlgorithm(dtmAlgorithm),
    mPointCloud(pointCloud),
    mOffset(offset),
    mDtmFile(dtmFile),
    mGSD(gsd),
    mDSM(dsm),
    mCrs(crs)
{

}

void DtmProcess::execute(tl::Progress *progressBar)
{
  tl::unusedParameter(progressBar);
  
  try {

    tl::Chrono chrono("DTM finished");
    chrono.run();

    //tl::Path dtm_path(mDtmFile.toStdString());
    tl::Path dtm_folder = mDtmFile.parentPath();
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
      csf.filter(mPointCloud.toString(), ground_path.toString(), out_ground_path.toString());

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
      
      std::unique_ptr<tl::ImageReader> image_reader_mds_ground = tl::ImageReaderFactory::create(mds_ground.toString());
      std::unique_ptr<tl::ImageReader> image_reader_mds_out_ground = tl::ImageReaderFactory::create(mds_out_ground.toString());
      std::unique_ptr<tl::ImageWriter> image_writer_mds_ground = tl::ImageWriterFactory::create(mDtmFile);

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

      DenseExport denseExport(mPointCloud);
      denseExport.setOffset(mOffset);
      denseExport.exportToCSV(csv_path.toString(), DenseExport::Fields::xyz, &bbox);

      tl::Size<int> size(static_cast<int>(bbox.width() / mGSD), static_cast<int>(bbox.height() / mGSD));

      mDtmAlgorithm->run(csv_path.toString(), mDtmFile, size);

    }
    
    chrono.stop();

    //emit dtmFinished();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
  }

}

} // graphos
