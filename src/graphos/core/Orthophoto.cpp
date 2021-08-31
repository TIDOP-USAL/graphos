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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "Orthophoto.h"

#include <tidop/core/exception.h>
#include <tidop/core/progress.h>
#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/ortho.h>
#include <tidop/vect/vectreader.h>
#include <tidop/graphic/layer.h>
#include <tidop/graphic/datamodel.h>

#include <opencv2/stitching.hpp>
#include <opencv2/imgcodecs.hpp>


 /// Factor de carga para la compensaci�n de exposici�n
constexpr double exposure_compensator_factor = 0.5;


std::vector<tl::WindowD> findGrid(const tl::Path &footprint_file)
{
  std::vector<tl::WindowD> grid;

  tl::WindowD window_all;

  std::unique_ptr<tl::VectorReader> vectorReader = tl::VectorReaderFactory::createReader(footprint_file);
  vectorReader->open();
  if (vectorReader->isOpen()) {
    if (vectorReader->layersCount() >= 1) {

      std::map<double, std::shared_ptr<tl::graph::GPolygon>> entities;
      std::shared_ptr<tl::graph::GLayer> layer = vectorReader->read(0);

      /// Busqueda ventana total

      double grid_step = -1;

      for (const auto &entity : *layer) {
        std::shared_ptr<tl::graph::GPolygon> polygon = std::dynamic_pointer_cast<tl::graph::GPolygon>(entity);
        std::shared_ptr<tl::TableRegister> data = polygon->data();
        tl::WindowD window = polygon->window();
        grid_step = std::min(window.width(), window.height());
        window_all = tl::joinWindow(window_all, window);
      }

      grid_step /= 3.;

      int grid_horizontal_size = static_cast<int>(window_all.width() / grid_step);
      int grid_vertical_size = static_cast<int>(window_all.height() / grid_step);

      double x_ini = window_all.pt1.x + grid_step / 2.;
      double y_ini = window_all.pt2.y - grid_step / 2.;

      grid.emplace_back(tl::PointD(x_ini, y_ini), grid_step * 2);

      tl::PointD point;
      for (size_t i = 0; i < grid_horizontal_size; i++) {

        point.x = x_ini + grid_step * i;

        for (size_t j = 0; j < grid_vertical_size; j++) {

          point.y = y_ini - grid_step * j;

          grid.emplace_back(point, grid_step * 2);

        }
      }

    }

  }

  return grid;
}

std::shared_ptr<tl::graph::GPolygon> bestImage(const tl::PointD &pt, const std::string &footprint_file)
{
  std::shared_ptr<tl::graph::GPolygon> footprint_image;

  std::unique_ptr<tl::VectorReader> vectorReader = tl::VectorReaderFactory::createReader(footprint_file);
  vectorReader->open();
  if (vectorReader->isOpen()) {

    if (vectorReader->layersCount() >= 1) {

      std::map<double, std::shared_ptr<tl::graph::GPolygon>> entities;
      std::shared_ptr<tl::graph::GLayer> layer = vectorReader->read(0);

      for (const auto &entity : *layer) {
        tl::graph::GraphicEntity::Type type = entity->type();
        if (type == tl::graph::GraphicEntity::Type::polygon_2d) {

          std::shared_ptr<tl::graph::GPolygon> polygon = std::dynamic_pointer_cast<tl::graph::GPolygon>(entity);
          if (polygon->isInner(pt)) {
            tl::PointD center = polygon->window().center();
            double distance = tl::distance(center, pt);
            entities[distance] = polygon;
          }

        } else {
          msgError("No es un fichero de huella de vuelo");
          break;
        }

      }

      if (!entities.empty()) {
        footprint_image = entities.begin()->second;
      }

    }

    vectorReader->close();

  }

  return footprint_image;
}

void findOptimalFootprint(const tl::Path &footprint_file,
                          std::vector<tl::WindowD> &grid,
                          const tl::Path &optimal_footprint_path,
                          const tl::geospatial::Crs &crs)
{
  std::map<std::string, std::shared_ptr<tl::graph::GPolygon>> clean_footprint;

  for (size_t i = 0; i < grid.size(); i++) {

    /// Busqueda de imagen mas centrada
    std::shared_ptr<tl::graph::GPolygon> polygon = bestImage(grid[i].center(), footprint_file.toString());
    if (polygon) {
      std::shared_ptr<tl::TableRegister> data = polygon->data();
      std::string ortho_to_compensate = data->value(0);
      clean_footprint[ortho_to_compensate] = polygon;
    }

  }

  msgInfo("Optimal footprint. %i retained images", clean_footprint.size());

  std::unique_ptr<tl::VectorWriter> vector_writer = tl::VectorWriterFactory::createWriter(optimal_footprint_path.toString());
  vector_writer->open();
  if (!vector_writer->isOpen())throw std::runtime_error("Vector open error");
  vector_writer->create();
  vector_writer->setCRS(crs);

  std::shared_ptr<tl::TableField> field(new tl::TableField("image",
                                                           tl::TableField::Type::STRING,
                                                           254));
  std::vector<std::shared_ptr<tl::TableField>> fields;
  fields.push_back(field);

  tl::graph::GLayer layer;
  layer.setName("footprint");
  layer.addDataField(field);

  for (const auto &footprint : clean_footprint) {
    std::shared_ptr<tl::TableRegister> data(new tl::TableRegister(fields));
    data->setValue(0, footprint.first);
    layer.push_back(footprint.second);
  }

  vector_writer->write(layer);

  vector_writer->close();
}

void orthoMosaic(tl::Path &optimal_footprint_path,
                 tl::Path &ortho_path,
                 double res_ortho,
                 tl::geospatial::Crs &crs,
                 std::vector<tl::WindowD> &grid)
{
  std::vector<std::string> compensated_orthos;
  std::vector<std::string> ortho_seams;
  std::vector<cv::Point> corners;
  std::vector<cv::Mat> mat_orthos;
  std::vector<cv::UMat> umat_orthos;
  std::vector<cv::UMat> ortho_masks;
  tl::WindowD window_all;

  msgInfo("Exposure compensator");

  //int type = cv::detail::ExposureCompensator::NO;
  int type = cv::detail::ExposureCompensator::GAIN;
  //int type = cv::detail::ExposureCompensator::GAIN_BLOCKS;
  //int type = cv::detail::ExposureCompensator::CHANNELS;
  //int type = cv::detail::ExposureCompensator::CHANNELS_BLOCKS;
  cv::Ptr<cv::detail::ExposureCompensator> compensator = cv::detail::ExposureCompensator::createDefault(type);

  std::unique_ptr<tl::VectorReader> vectorReader;
  vectorReader = tl::VectorReaderFactory::createReader(optimal_footprint_path.toString());
  vectorReader->open();

  if (vectorReader->isOpen()) {

    if (vectorReader->layersCount() >= 1) {

      std::map<double, std::shared_ptr<tl::graph::GPolygon>> entities;
      std::shared_ptr<tl::graph::GLayer> layer = vectorReader->read(0);

      for (const auto &entity : *layer) {

        tl::graph::GraphicEntity::Type type = entity->type();
        if (type == tl::graph::GraphicEntity::Type::polygon_2d) {

          /// se carga la primera imagen y se busca las que intersectan
          std::shared_ptr<tl::graph::GPolygon> polygon = std::dynamic_pointer_cast<tl::graph::GPolygon>(entity);
          std::shared_ptr<tl::TableRegister> data = polygon->data();
          std::string ortho_to_compensate = data->value(0);
          tl::WindowD window = polygon->window();
          tl::PointD center = window.center();

          /// Busqueda de imagenes que intersectan

          std::vector<std::string> orthos;
          orthos.push_back(ortho_to_compensate);
          std::vector<tl::WindowD> windows;
          windows.push_back(window);

          for (const auto &entity2 : *layer) {
            std::shared_ptr<tl::graph::GPolygon> polygon2 = std::dynamic_pointer_cast<tl::graph::GPolygon>(entity2);
            std::shared_ptr<tl::TableRegister> data = polygon2->data();
            std::string orto = data->value(0);

            if (orto != ortho_to_compensate) {

              //if (polygon2->isInner(polygon->at(0)) || 
              //    polygon2->isInner(polygon->at(1)) || 
              //    polygon2->isInner(polygon->at(2)) || 
              //    polygon2->isInner(polygon->at(3))) {
              /// No se si ser� suficiente o tengo que seleccionar todas las imagenes que intersecten...
              if (polygon2->isInner(center)) {
                orthos.push_back(orto);
                windows.push_back(polygon2->window());
                window_all = joinWindow(window_all, polygon2->window());
              }

            }

          }

          size_t n_orthos = orthos.size();
          corners.resize(n_orthos);
          ortho_masks.resize(n_orthos);
          umat_orthos.resize(n_orthos);
          mat_orthos.resize(n_orthos);

          /// Aplicar un factor de escala para el calculo de la compensaci�n de exposici�n
          for (size_t i = 0; i < n_orthos; i++) {

            std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::createReader(orthos[i]);
            image_reader->open();
            if (!image_reader->isOpen()) throw std::runtime_error("Image open error");
            cv::Mat image = image_reader->read(exposure_compensator_factor, exposure_compensator_factor);
            mat_orthos[i] = image.clone();
            double scale = image_reader->georeference().scaleX();

            /// Esquinas
            corners[i].x = (windows[i].pt1.x - window_all.pt1.x) * exposure_compensator_factor / scale;
            corners[i].y = (window_all.pt2.y - windows[i].pt2.y) * exposure_compensator_factor / scale;

            /// La mascara deber�a leerse si se cre� en la generaci�n del MDS.
            ortho_masks[i].create(image.size(), CV_8U);
            cv::Mat gray;
            cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
            ortho_masks[i].setTo(cv::Scalar::all(0));
            ortho_masks[i].setTo(cv::Scalar::all(255), gray > 0);
          }

          cv::InputArrayOfArrays(mat_orthos).getUMatVector(umat_orthos);
          compensator->feed(corners, umat_orthos, ortho_masks);

          msgInfo("Seam finder");

          cv::Ptr<cv::detail::SeamFinder> seam_finder;
          seam_finder = cv::makePtr<cv::detail::NoSeamFinder>();
          //seam_finder = cv::makePtr<cv::detail::VoronoiSeamFinder>();
          //seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR);
          //seam_finder = cv::makePtr<cv::detail::DpSeamFinder>(cv::detail::DpSeamFinder::COLOR_GRAD);
          seam_finder->find(umat_orthos, corners, ortho_masks);
          umat_orthos.clear();
          mat_orthos.clear();

          std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::createReader(ortho_to_compensate);
          image_reader->open();
          if (!image_reader->isOpen()) throw std::runtime_error("Image open error");
          cv::Mat compensate_image = image_reader->read();

          /// Se compensa la imagen
          cv::Point corner = corners[0] / exposure_compensator_factor;
          cv::Mat gray;
          cv::cvtColor(compensate_image, gray, cv::COLOR_BGR2GRAY);
          cv::Mat mask_full_size(compensate_image.size(), CV_8U);
          mask_full_size.setTo(cv::Scalar::all(0));
          mask_full_size.setTo(cv::Scalar::all(255), gray > 0);
          cv::Mat element = getStructuringElement(cv::MorphShapes::MORPH_RECT,
                                                  cv::Size(2 * 2 + 1, 2 * 2 + 1),
                                                  cv::Point(2, 2));
          cv::erode(mask_full_size, mask_full_size, element);
          compensator->apply(0, corner, compensate_image, mask_full_size);

          tl::Path orto_compensate(ortho_to_compensate);
          std::string name = orto_compensate.baseName() + "_compensate.png";
          orto_compensate.replaceFileName(name);
          std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::createWriter(orto_compensate.toString());
          image_writer->open();
          if (image_writer->isOpen()) {
            image_writer->create(image_reader->rows(), image_reader->cols(), image_reader->channels(), image_reader->dataType());
            image_writer->setCRS(image_reader->crs());
            image_writer->setGeoreference(image_reader->georeference());
            image_writer->write(compensate_image);
            image_writer->close();
            msgInfo("Image Compensate: %s", orto_compensate.fileName().c_str());
            compensated_orthos.push_back(orto_compensate.toString());
          }

          /// 2 - Busqueda de costuras (seam finder)

          cv::Mat mask_finder = ortho_masks[0].getMat(cv::ACCESS_READ);
          cv::erode(mask_finder, mask_finder, element);
          cv::resize(mask_finder, mask_finder, compensate_image.size());
          mask_finder = mask_finder & mask_full_size;

          tl::Path orto_seam(ortho_to_compensate);
          name = orto_seam.baseName() + "_seam.tif";
          orto_seam.replaceFileName(name);
          image_writer = tl::ImageWriterFactory::createWriter(orto_seam.toString());
          image_writer->open();
          if (image_writer->isOpen()) {
            image_writer->create(image_reader->rows(), image_reader->cols(), 1, image_reader->dataType());
            image_writer->setCRS(image_reader->crs());
            image_writer->setGeoreference(image_reader->georeference());
            image_writer->write(mask_finder);
            image_writer->close();
            msgInfo("Image seam: %s", orto_seam.fileName().c_str());
            ortho_seams.push_back(orto_seam.toString());
          }

          image_reader->close();

        } else {
          msgError("No es un fichero de huella de vuelo");
          return;
        }

      }

    }

    vectorReader->close();
  }

  /// 3 - mezcla (blender)

  bool try_cuda = false;
  //int blender_type = cv::detail::Blender::FEATHER;
  int blender_type = cv::detail::Blender::MULTI_BAND;
  cv::Ptr<cv::detail::Blender> blender;
  float blend_strength = 5;

  tl::Path ortho_final(ortho_path);
  ortho_final.append("ortho.tif");
  std::unique_ptr<tl::ImageWriter> image_writer = tl::ImageWriterFactory::createWriter(ortho_final.toString());
  image_writer->open();
  int cols = static_cast<int>(std::round(window_all.width() / res_ortho));
  int rows = static_cast<int>(std::round(window_all.height() / res_ortho));

  if (image_writer->isOpen()) {
    image_writer->create(rows, cols, 3, tl::DataType::TL_8U);
    image_writer->setCRS(crs);
    tl::Affine<tl::PointD> affine_ortho(window_all.pt1.x,
                                        window_all.pt2.y,
                                        res_ortho, -res_ortho, 0.0);
    image_writer->setGeoreference(affine_ortho);

    for (size_t i = 0; i < grid.size(); i++) {

      blender = cv::detail::Blender::createDefault(blender_type, try_cuda);

      int cols_grid = static_cast<int>(std::round(grid[i].width() / res_ortho));
      int rows_grid = static_cast<int>(std::round(grid[i].height() / res_ortho));
      cv::Rect rect(0, 0, cols_grid, rows_grid);

      //cv::Size dst_sz = rect.size();
      float blend_width = sqrt(static_cast<float>(rect.area())) * blend_strength / 100.f;

      if (blend_width < 1.f) {
        blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, try_cuda);
      } else if (blender_type == cv::detail::Blender::MULTI_BAND) {
        cv::detail::MultiBandBlender *multi_band_blender = dynamic_cast<cv::detail::MultiBandBlender *>(blender.get());
        multi_band_blender->setNumBands(4/*static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.)*/);
        msgInfo("Multi-band blender, number of bands: %i", multi_band_blender->numBands());
      } else if (blender_type == cv::detail::Blender::FEATHER) {
        cv::detail::FeatherBlender *feather_blender = dynamic_cast<cv::detail::FeatherBlender *>(blender.get());
        feather_blender->setSharpness(0.02f/*1.f / blend_width*/);
        msgInfo("Feather blender, sharpness: %f", feather_blender->sharpness());
      }

      blender->prepare(rect);

      for (size_t j = 0; j < compensated_orthos.size(); j++) {
        std::unique_ptr<tl::ImageReader> image_reader = tl::ImageReaderFactory::createReader(compensated_orthos[j]);
        std::unique_ptr<tl::ImageReader> image_reader_seam = tl::ImageReaderFactory::createReader(ortho_seams[j]);
        image_reader->open();
        image_reader_seam->open();
        if (!image_reader->isOpen() || !image_reader_seam->isOpen()) throw std::runtime_error("Image open error");

        if (!intersectWindows(image_reader->window(), grid[i]) ||
            !intersectWindows(image_reader_seam->window(), grid[i])) continue;

        double scale_x = image_reader->georeference().scaleX();
        double scale_y = image_reader->georeference().scaleY();
        double read_scale_x = scale_x / res_ortho;
        double read_scale_y = scale_y / res_ortho;
        tl::PointD p1 = image_reader->georeference().transform(grid[i].pt1, tl::Transform::Order::inverse);
        tl::PointD p2 = image_reader->georeference().transform(grid[i].pt2, tl::Transform::Order::inverse);
        tl::WindowI window_to_read(static_cast<tl::PointI>(p1), static_cast<tl::PointI>(p2));
        window_to_read.normalized();

        tl::Affine<tl::PointI> affine;
        cv::Mat compensate_image;
        cv::Mat seam_image;
        try {
          compensate_image = image_reader->read(grid[i], read_scale_x, read_scale_y, &affine);
          seam_image = image_reader_seam->read(grid[i], read_scale_x, read_scale_y);
        } catch (...) {
          continue;
        }

        if (!compensate_image.empty() && !seam_image.empty()) {

          msgInfo("Ortho grid %i", i, compensated_orthos[j].c_str());

          cv::Mat compensate_image_16s;
          compensate_image.convertTo(compensate_image_16s, CV_16S);
          compensate_image.release();

          cv::Rect rect = cv::Rect(affine.tx, affine.ty, compensate_image_16s.cols, compensate_image_16s.rows);
          blender->feed(compensate_image_16s, seam_image, rect.tl());
        }
      }
      cv::Mat ortho_blend;
      cv::Mat mask_blend;
      blender->blend(ortho_blend, mask_blend);
      ortho_blend.convertTo(ortho_blend, CV_8U);

      affine_ortho.transform(grid[i].pt1);
      affine_ortho.transform(grid[i].pt2);
      tl::PointD p1 = affine_ortho.transform(grid[i].pt1, tl::Transform::Order::inverse);
      tl::PointD p2 = affine_ortho.transform(grid[i].pt2, tl::Transform::Order::inverse);
      tl::WindowI window_to_write(static_cast<tl::PointI>(p1), static_cast<tl::PointI>(p2));
      window_to_write.normalized();
      image_writer->write(ortho_blend, window_to_write);

    }

    image_writer->close();
  }
}

namespace graphos
{


/// Definir valores por defecto
constexpr auto DefaultOrthophotoResolution = 0.01;



/* OrthophotoParameters */

OrthophotoParameters::OrthophotoParameters()
  : mResolution(DefaultOrthophotoResolution)
{
}

OrthophotoParameters::~OrthophotoParameters()
{
}

double OrthophotoParameters::resolution() const 
{
  return mResolution;
}

void OrthophotoParameters::setResolution(double resolution) 
{
  mResolution = resolution;
}

/*
int OrthophotoParameters::[propiety]() const
{
  return m[propiety];
}


void OrthophotoParameters::set[Propiety](int [propiety])
{
  m[propiety] = [propiety];
}
*/

void OrthophotoParameters::clear()
{
  mResolution = DefaultOrthophotoResolution;
}


/* OrthophotoAlgorithm */


OrthophotoAlgorithm::OrthophotoAlgorithm()
{
}

OrthophotoAlgorithm::OrthophotoAlgorithm(double resolution,
                                         const std::vector<tl::Photo> &photos,
                                         const QString &orthoPath,
                                         const QString &mdt,
                                         const QString &epsg)
  : mPhotos(photos),
    mOrthoPath(orthoPath),
    mMdt(mdt),
    mEpsg(epsg)
{
  OrthophotoParameters::setResolution(resolution);
}

OrthophotoAlgorithm::~OrthophotoAlgorithm()
{
}

void OrthophotoAlgorithm::run()
{

  tl::Path ortho_path(mOrthoPath.toStdWString());
  tl::Path footprint_file(ortho_path);
  footprint_file.append("footprint.shp");
  tl::Path graph_orthos = tl::Path(footprint_file).replaceBaseName("graph_orthos");
  tl::geospatial::Crs crs(mEpsg.toStdString());

  tl::geospatial::OrthoimageProcess ortho_process(mPhotos,
                                                  mMdt.toStdString(), 
                                                  ortho_path, 
                                                  graph_orthos, 
                                                  crs, 
                                                  footprint_file, 
                                                  OrthophotoParameters::resolution(),
                                                  0.4);
  ortho_process.run();

  std::vector<tl::WindowD> grid = findGrid(graph_orthos);

  tl::Path optimal_footprint_path(graph_orthos);
  std::string name = optimal_footprint_path.baseName() + "_optimal";
  optimal_footprint_path.replaceBaseName(name);
  findOptimalFootprint(graph_orthos, grid, optimal_footprint_path, crs);

  orthoMosaic(optimal_footprint_path, ortho_path, OrthophotoParameters::resolution(), crs, grid);

}

void OrthophotoAlgorithm::setPhotos(const std::vector<tl::Photo> &photos)
{
  mPhotos = photos;
}

void OrthophotoAlgorithm::setOrthoPath(const QString &orthoPath)
{
  mOrthoPath = orthoPath;
}

void OrthophotoAlgorithm::setMdt(const QString &mdt)
{
  mMdt = mdt;
}

void OrthophotoAlgorithm::setCrs(const QString &epsg)
{
  mEpsg = epsg;
}

} // namespace graphos