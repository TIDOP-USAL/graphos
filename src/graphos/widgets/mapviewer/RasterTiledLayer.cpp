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

#include "graphos/widgets/mapviewer/RasterTiledLayer.h"
#include "graphos/core/utils.h"

/* TidopLib */
#include <tidop/rastertools/io/Reader.h>
#include <tidop/geometry/algorithms/measurement/Distance.h>
#include <tidop/geometry/algorithms/spatial/Intersection.h>
#include <tidop/geometry/transform/Affine.h>
#include <tidop/geometry/transform/estimators/Affine2D.h>

/* QGeoView */
#include <QGeoView/Raster/QGVImage.h>

/* GDAL */
#include <gdal.h>
#include <gdal_priv.h>
#include <gdalwarper.h>
#include <ogr_spatialref.h>

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>



namespace graphos
{

namespace
{

void copyMatToImage(const cv::Mat &source, QImage &destination, int offsetX, int offsetY)
{
    const int channels = source.channels();
    if (source.depth() != CV_8U || (channels != 1 && channels != 3 && channels != 4)) {
        throw std::runtime_error("Unsupported raster tile pixel format");
    }

    const int sourceX = std::max(0, -offsetX);
    const int sourceY = std::max(0, -offsetY);
    const int endX = std::min(source.cols, destination.width() - offsetX);
    const int endY = std::min(source.rows, destination.height() - offsetY);

    for (int y = sourceY; y < endY; ++y) {
        const auto *sourceRow = source.ptr<uint8_t>(y);
        auto *destinationRow = destination.scanLine(offsetY + y);
        for (int x = sourceX; x < endX; ++x) {
            const auto *pixel = sourceRow + x * channels;
            const uint8_t blue = pixel[0];
            const uint8_t green = channels == 1 ? pixel[0] : pixel[1];
            const uint8_t red = channels == 1 ? pixel[0] : pixel[2];
            uint8_t alpha = channels == 4 ? pixel[3] : 255;

            if (red == 0 && green == 0 && blue == 0) {
                alpha = 0;
            }

            auto *rgba = destinationRow + (offsetX + x) * 4;
            rgba[0] = red;
            rgba[1] = green;
            rgba[2] = blue;
            rgba[3] = alpha;
        }
    }
}

} // namespace

RasterTiledLayer::RasterTiledLayer(const QString &tifPath, bool dem)
  : mDEM(dem)
{
    setZValue(-1);

    tl::Path file_path(tifPath.toStdWString());
    reader = std::make_unique<tl::RasterReader>(file_path);
    if (!reader->isOpen()) {
        throw std::runtime_error("The GeoTIFF file could not be opened: " + file_path.toUtf8());
    }
    auto georeference = reader->georeference();
    mAffineImageToWorld = georeference.inverse();
    auto window = reader->window();
    mRasterRows = reader->rows();
    mRasterCols = reader->cols();
    mNoDataValue = reader->noDataValue(&mHasNoDataValue);
    mCellSize = std::abs(georeference.scale().x());
    mThreadPool.setMaxThreadCount(std::max(1, std::min(2, QThread::idealThreadCount())));

    OGRCoordinateTransformation *transform = nullptr;

    // Obtener CRS del GeoTiff
    //const OGRSpatialReference *sourceRef = mDataset->GetSpatialRef();
    OGRSpatialReference sourceRef;
    sourceRef.importFromWkt(reader->crsWkt().c_str());
    if (sourceRef.IsEmpty()) {
        throw std::runtime_error("GeoTIFF has no usable CRS");
    }

    OGRSpatialReference targetRef;
    targetRef.importFromEPSG(4326);

    if (!sourceRef.IsEmpty() && !sourceRef.IsSame(&targetRef)) {
        transform = OGRCreateCoordinateTransformation(&sourceRef, &targetRef);
        if (!transform) {
            throw std::runtime_error("Could not create raster CRS transformation");
        }

        constexpr int edgeSamples = 32;
        const double minX = window.min().x();
        const double minY = window.min().y();
        const double maxX = window.max().x();
        const double maxY = window.max().y();
        std::vector<double> xCoordinates;
        std::vector<double> yCoordinates;
        xCoordinates.reserve((edgeSamples + 1) * 4);
        yCoordinates.reserve((edgeSamples + 1) * 4);

        for (int i = 0; i <= edgeSamples; ++i) {
            const double t = static_cast<double>(i) / edgeSamples;
            const double x = minX + t * (maxX - minX);
            const double y = minY + t * (maxY - minY);
            xCoordinates.push_back(x);
            yCoordinates.push_back(minY);
            xCoordinates.push_back(x);
            yCoordinates.push_back(maxY);
            xCoordinates.push_back(minX);
            yCoordinates.push_back(y);
            xCoordinates.push_back(maxX);
            yCoordinates.push_back(y);
        }

        const bool transformed = transform->Transform(static_cast<int>(xCoordinates.size()),
                                                       xCoordinates.data(), yCoordinates.data());

        OCTDestroyCoordinateTransformation(transform);
        transform = nullptr;
        if (!transformed) {
            throw std::runtime_error("Could not transform raster bounds to EPSG:4326");
        }

        const auto bounds = std::minmax_element(xCoordinates.begin(), xCoordinates.end());
        const auto yBounds = std::minmax_element(yCoordinates.begin(), yCoordinates.end());
        window.min().x() = *bounds.first;
        window.min().y() = *yBounds.first;
        window.max().x() = *bounds.second;
        window.max().y() = *yBounds.second;
    }

    mGeoExtent = sourceRef.IsSame(&targetRef)
        ? QGV::GeoRect(window.min().y(), window.min().x(), window.max().y(), window.max().x())
        : QGV::GeoRect(window.min().x(), window.min().y(), window.max().x(), window.max().y());

    const char *auth_name = sourceRef.GetAuthorityName(nullptr);
    const char *auth_code = sourceRef.GetAuthorityCode(nullptr);
    if (auth_name && std::string(auth_name) == "EPSG" && auth_code) {
        mEpsgSource = std::string(auth_name).append(":").append(auth_code);
        OGRSpatialReference webMercatorRef;
        webMercatorRef.importFromEPSG(3857);
        mCrsTransform = !sourceRef.IsSame(&webMercatorRef);
    } else {
        throw std::runtime_error("Raster display currently requires an EPSG-authorized CRS");
    }
    
    // Calculo de la escala máxima
    {
        auto rows = reader->rows();
        auto cols = reader->cols();
        auto p1 = mAffineImageToWorld.transform(tl::Point2d(0., 0.));
        auto p2 = mAffineImageToWorld.transform(tl::Point2d(cols, rows));
        
        mGSD = std::min(cols / std::abs(p1.x() - p2.x()), rows / std::abs(p1.y() - p2.y()));
    }

    // Se comprueba si la imagen es un DEM (tambien podría ser imagen termografica, etc)
    // Se calcula el min y max de la imagen
    if (reader->channels() == 1 && reader->dataType() != tl::DataType::TL_8U && reader->dataType() != tl::DataType::TL_8S) {

        bool exist_nodata = false;
        double nodata_value = reader->noDataValue(&exist_nodata);
        if (!exist_nodata) nodata_value = tl::NoData<float>;

        cv::Mat dem = reader->read(0.1, 0.1);
        cv::Mat mask;
        cv::inRange(dem, cv::Scalar::all(nodata_value), cv::Scalar::all(nodata_value), mask);
        cv::bitwise_not(mask, mask);
        cv::minMaxLoc(dem, &mMinMax.first, &mMinMax.second, nullptr, nullptr, mask);

    }
}

RasterTiledLayer::~RasterTiledLayer()
{
    for (auto it = mRequests.begin(); it != mRequests.end(); ++it) {
        it->cancelled->store(true);
    }
    mRequests.clear();
    mThreadPool.waitForDone();
    reader->close();
}

QGV::GeoRect RasterTiledLayer::maxGeoExtent() const
{
    return mGeoExtent;
}

int RasterTiledLayer::minZoomlevel() const
{
    return 10; 
}

int RasterTiledLayer::maxZoomlevel() const 
{ 
    return 25; 
}

void RasterTiledLayer::onProjection(QGVMap *geoMap)
{
    QGVLayerTiles::onProjection(geoMap);
    mProjectedExtent = geoMap->getProjection()->geoToProj(mGeoExtent);

    // Limitar área activa a donde hay datos
    mTileGridBounds = QRect(QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.topLeft()).pos(),
                            QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.bottomRight()).pos());
}



void RasterTiledLayer::request(const QGV::GeoTilePos &tilePos)
{
    auto tilePosParent = tilePos.parent(minZoomlevel()).pos();
    if (!mTileGridBounds.contains(tilePosParent)) {
        return;
    }

    const QGV::GeoRect tileGeoRect = tilePos.toGeoRect();
    if (!mGeoExtent.intersects(tileGeoRect) || mRequests.contains(tilePos)) {
        return;
    }

    const QRectF tileProjRect = getMap()->getProjection()->geoToProj(tileGeoRect);
    const quint64 requestId = ++mNextRequestId;
    auto cancelled = std::make_shared<std::atomic_bool>(false);
    mRequests.insert(tilePos, {requestId, cancelled});

    mThreadPool.start(QRunnable::create(
        [this, tilePos, tileProjRect, requestId, cancelled]() {
            if (cancelled->load()) {
                return;
            }

            QImage image = renderTile(tilePos, tileProjRect);
            if (cancelled->load()) {
                return;
            }

            QMetaObject::invokeMethod(this,
                [this, tilePos, requestId, image = std::move(image)]() mutable {
                    finishTile(tilePos, requestId, std::move(image));
                },
                Qt::QueuedConnection);
        }));
}

void RasterTiledLayer::finishTile(const QGV::GeoTilePos &tilePos, quint64 requestId, QImage image)
{
    const auto requestIt = mRequests.find(tilePos);
    if (requestIt == mRequests.end() || requestIt->id != requestId) {
        return;
    }
    mRequests.erase(requestIt);

    if (image.isNull()) {
        return;
    }

    auto *tile = new QGVImage();
    tile->setGeometry(tilePos.toGeoRect());
    tile->loadImage(image);
    tile->setSelectable(false);
    onTile(tilePos, tile);
}

void RasterTiledLayer::cancel(const QGV::GeoTilePos &tilePos)
{
    auto requestIt = mRequests.find(tilePos);
    if (requestIt == mRequests.end()) {
        return;
    }
    requestIt->cancelled->store(true);
    mRequests.erase(requestIt);
}

QImage RasterTiledLayer::renderTile(const QGV::GeoTilePos &tilePos, const QRectF &tile_proj_rect)
{
    try {

        const QRectF rect_image_epsg_3857 = mProjectedExtent;

        // Establezco tamaño al doble porque se ve muy mal en el nivel de zoom remuestreado
        int tileSize = 512;// 256; // Tamaño del tile en píxeles
        int tile_size_x_dst = tileSize;
        int tile_size_y_dst = tileSize;


        QImage tileImage(tileSize, tileSize, QImage::Format_RGBA8888);
        tileImage.fill(Qt::transparent);

        if (mCrsTransform) {

            // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
            // la Y sale con el signo cambiado
            // Puntos de los extremos del tile en coordenadas proyectadas
            std::vector<tl::Point2d> points_tile(4);
            points_tile[0] = tl::Point2d(tile_proj_rect.left(), -tile_proj_rect.top());
            points_tile[1] = tl::Point2d(tile_proj_rect.right(), -tile_proj_rect.top());
            points_tile[2] = tl::Point2d(tile_proj_rect.right(), -tile_proj_rect.bottom());
            points_tile[3] = tl::Point2d(tile_proj_rect.left(), -tile_proj_rect.bottom());

            std::vector<tl::Point2d> points_image_epsg_3857(4);
            points_image_epsg_3857[0] = tl::Point2d(rect_image_epsg_3857.left(), -rect_image_epsg_3857.top());
            points_image_epsg_3857[1] = tl::Point2d(rect_image_epsg_3857.right(), -rect_image_epsg_3857.top());
            points_image_epsg_3857[2] = tl::Point2d(rect_image_epsg_3857.right(), -rect_image_epsg_3857.bottom());
            points_image_epsg_3857[3] = tl::Point2d(rect_image_epsg_3857.left(), -rect_image_epsg_3857.bottom());
            

            // 1 - Tile window
            std::vector<tl::Point2d> tile_window(4);
            tile_window[0] = tl::Point2d(0., 0.);
            tile_window[1] = tl::Point2d(tileSize, 0.);
            tile_window[2] = tl::Point2d(tileSize, tileSize);
            tile_window[3] = tl::Point2d(0., tileSize);

            // 2 - Affine transformation
            auto affine_image_epsg_3857 = tl::Affine2DEstimator<double>::estimate(tile_window, points_tile);

            // 3 - Ventana intersección
            tl::BoundingBox2d window_tile_epsg_3857(points_tile);
            tl::BoundingBox2d window_image_epsg_3857(points_image_epsg_3857);
            tl::BoundingBox2d window_read_3857 = tl::intersection(window_tile_epsg_3857, window_image_epsg_3857);

            // 4 - Con la ventana intersección y la transformación afin calcular la zona con datos dentro de la tesela
            tl::BoundingBox2i window_image_to_read_3857;
            window_image_to_read_3857.min() = static_cast<tl::Point2i>(affine_image_epsg_3857.inverse().transform(window_read_3857.min()));
            window_image_to_read_3857.max() = static_cast<tl::Point2i>(affine_image_epsg_3857.inverse().transform(window_read_3857.max()));
            window_image_to_read_3857.normalized();

            tl::Point2i offset = window_image_to_read_3857.min();
            int width = window_image_to_read_3857.width()/* - offset.x*/;
            int height = window_image_to_read_3857.height()/* - offset.y*/;
            if (width <= 0 || height <= 0) return {};

            //5 - Calcular ventana a leer en coordenadas origen.

            std::vector<double> sourceX{
                window_read_3857.min().x(), window_read_3857.max().x(),
                window_read_3857.max().x(), window_read_3857.min().x()
            };
            std::vector<double> sourceY{
                window_read_3857.max().y(), window_read_3857.max().y(),
                window_read_3857.min().y(), window_read_3857.min().y()
            };
            OGRSpatialReference mapCrs;
            OGRSpatialReference rasterCrs;
            if (mapCrs.importFromEPSG(3857) != OGRERR_NONE ||
                rasterCrs.SetFromUserInput(mEpsgSource.c_str()) != OGRERR_NONE) {
                throw std::runtime_error("Could not initialize raster tile CRS transformation");
            }
            mapCrs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
            rasterCrs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
            std::unique_ptr<OGRCoordinateTransformation, decltype(&OCTDestroyCoordinateTransformation)> transform(
                OGRCreateCoordinateTransformation(&mapCrs, &rasterCrs),
                &OCTDestroyCoordinateTransformation);
            if (!transform || !transform->Transform(4, sourceX.data(), sourceY.data())) {
                throw std::runtime_error("Could not transform raster tile bounds");
            }

            std::vector<tl::Point3d> points_source;
            points_source.reserve(sourceX.size());
            for (size_t i = 0; i < sourceX.size(); ++i) {
                points_source.emplace_back(sourceX[i], sourceY[i], 0.);
            }

            // Aqui hay que calcular la distancia entre los puntos para que este bien la escala
            double scale_src = 1.0;
            {
                auto p1 = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[0]));
                auto p2 = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[1]));
                auto p3 = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[3]));

                auto d1 = tl::distance(p1, p2);
                auto d2 = tl::distance(p1, p3);
                scale_src = std::min(tile_size_x_dst / d1, tile_size_y_dst / d2);
            }


            tl::BoundingBox3d source_window(points_source);

            // Hay que expandir la ventana para evitar problemas en los bordes
            auto p1 = mAffineImageToWorld.transform(tl::Point2d(source_window.min().x(), source_window.max().y()));
            auto p2 = mAffineImageToWorld.transform(tl::Point2d(source_window.max().x(), source_window.min().y()));
            tl::Rect<int> rect_src(static_cast<tl::Point2i>(p1), static_cast<tl::Point2i>(p2));

            double xSize = rect_src.width();
            double ySize = rect_src.height();

            tl::Rect<int> rect_full_image(0, 0, mRasterCols, mRasterRows);
            tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_src);

            if (rect_to_read.width() <= 0 || rect_to_read.height() <= 0) return {};

            cv::Mat mat_source;
            {
                std::lock_guard<std::mutex> lock(mReaderMutex);
                mat_source = reader->read(scale_src, scale_src, rect_to_read);
            }

            if (mat_source.empty()) return {};

            cv::Mat gray;
            if (mat_source.channels() == 3) {
                cv::cvtColor(mat_source, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = mat_source;
            }

            // Cuenta cuántos píxeles son distintos de 0
            if (cv::countNonZero(gray) == 0) return {};
            
            if (mDEM) {

                buildDEM(mat_source);

            } else if (mat_source.channels() == 1) {

                cv::Mat temp;
                cv::Mat mask;
                if (mHasNoDataValue) {
                    cv::inRange(mat_source, cv::Scalar::all(mNoDataValue), cv::Scalar::all(mNoDataValue), mask);
                    cv::bitwise_not(mask, mask);
                } else {
                    mask = cv::Mat(mat_source.size(), CV_8U, cv::Scalar(255));
                }

                if (mMinMax.second > mMinMax.first) {

                    double scale = 255.0 / (mMinMax.second - mMinMax.first);
                    double shift = -mMinMax.first * scale;
                    mat_source.convertTo(temp, CV_8U, scale, shift);
                    temp.copyTo(mat_source, mask);

                } else {
                    cv::normalize(mat_source, mat_source, 0., 255., cv::NORM_MINMAX, CV_8U, mask);
                }

                
                cv::cvtColor(mat_source, mat_source, cv::COLOR_GRAY2BGR);

            }

            std::vector<cv::Point2f> src_points_img(4);

            // Calcular puntos en coordenadas de píxel de mat_source (relativos a rect_to_read.topLeft)
            // 1) obtenemos las coordenadas en píxeles (sin escalar) en la referencia completa:
            auto src_p0_full = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[0]));
            auto src_p1_full = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[1]));
            auto src_p2_full = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[2]));
            auto src_p3_full = mAffineImageToWorld.transform(static_cast<tl::Point2d>(points_source[3]));
            // Estas son coordenadas en píxeles dentro de la imagen completa (sin recorte).

            // 2) restamos la esquina superior izquierda de rect_to_read (en píxeles) para tener coords relativas a mat_source
            tl::Point2d rectToReadTopLeft = static_cast<tl::Point2d>(rect_to_read.topLeft());

            tl::Point2d p0_rel = (src_p0_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p1_rel = (src_p1_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p2_rel = (src_p2_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p3_rel = (src_p3_full - rectToReadTopLeft) * scale_src;

            // Ahora construimos src_points_img con esos puntos (flotantes)
            src_points_img[0] = cv::Point2f(static_cast<float>(p0_rel.x()), static_cast<float>(p0_rel.y()));
            src_points_img[1] = cv::Point2f(static_cast<float>(p1_rel.x()), static_cast<float>(p1_rel.y()));
            src_points_img[2] = cv::Point2f(static_cast<float>(p2_rel.x()), static_cast<float>(p2_rel.y()));
            src_points_img[3] = cv::Point2f(static_cast<float>(p3_rel.x()), static_cast<float>(p3_rel.y()));

            // 2. Coordenadas destino en la tile de salida(EPSG:3857, 256x256 px)
            std::vector<cv::Point2f> dst_points = {
                {0.f, 0.f}, // top-left
                {static_cast<float>(width), 0.f}, // top-right
                {static_cast<float>(width), static_cast<float>(height)}, // bottom-right
                {0.f, static_cast<float>(height)}  // bottom-left
            };

            //// 3. Calcular homografía
            cv::Mat H = cv::getPerspectiveTransform(src_points_img, dst_points);

            // 5. Aplicar la transformación
            cv::Mat tileMat(height, width, CV_8UC3, cv::Scalar(0, 0, 0));

            cv::warpPerspective(mat_source, tileMat, H, tileMat.size(),
                                cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));

            // Establecer transparencia en píxeles negros (r,g,b == 0)
            copyMatToImage(tileMat, tileImage, offset.x(), offset.y());

        } else {

            // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
            // la Y sale con el signo cambiado
            auto p1 = mAffineImageToWorld.transform(tl::Point2d(tile_proj_rect.left(), -tile_proj_rect.top()));
            auto p2 = mAffineImageToWorld.transform(tl::Point2d(tile_proj_rect.right(), -tile_proj_rect.bottom()));

            int xOffset = std::floor(std::min(p1.x(), p2.x()));
            int yOffset = std::floor(std::min(p1.y(), p2.y()));
            int xSize = std::ceil(std::abs(p2.x() - p1.x()));
            int ySize = std::ceil(std::abs(p2.y() - p1.y()));

            double sx = tile_size_x_dst / static_cast<double>(xSize);
            double sy = tile_size_y_dst / static_cast<double>(ySize);

            int tile_offset_x = 0;
            int tile_offset_y = 0;
            auto rows = mRasterRows;
            auto cols = mRasterCols;
            if (xOffset < 0) {
                xSize = xSize + xOffset;
                xOffset = 0;
                tile_size_x_dst = sx * xSize;
                tile_offset_x = tileSize - tile_size_x_dst;
            }
            if (yOffset < 0) {
                ySize = ySize + yOffset;
                yOffset = 0;
                tile_size_y_dst = sy * ySize;
                tile_offset_y = tileSize - tile_size_y_dst;
            }

            if (xOffset + xSize > cols) {
                xSize = cols - xOffset;
                tile_size_x_dst = sx * xSize;
            }

            if (yOffset + ySize > rows) {
                ySize = rows - yOffset;
                tile_size_y_dst = sy * ySize;
            }

            tl::Rect<int> rect_to_read(xOffset, yOffset, xSize, ySize);
            tl::Size<int> size_to_read(tile_size_x_dst, tile_size_y_dst);
            cv::Mat mat_src;
            {
                std::lock_guard<std::mutex> lock(mReaderMutex);
                mat_src = reader->read(rect_to_read, size_to_read);
            }

            if (mat_src.empty()) return {};

            // Calcular tamaño en píxeles del tile

            copyMatToImage(mat_src, tileImage, tile_offset_x, tile_offset_y);

        }


        tileImage.setDevicePixelRatio(2.);
        return tileImage;

    } catch (const std::exception &e) {
        qWarning() << "Failed to render raster tile" << tilePos.pos() << e.what();
        return {};
    }
}

void RasterTiledLayer::buildDEM(cv::Mat &mat_source)
{
    // Parámetros. Se tienen que establecer en los ajustes de visualización
    double mAltitude = 45.0;   // grados sobre el horizonte
    double mAzimuth = 315.0;   // grados desde el norte, sentido horario
    bool mColorize = true;

    double global_min = mMinMax.first;
    double global_max = mMinMax.second;

    // Copia y asegura precisión
    cv::Mat dem64;
    mat_source.convertTo(dem64, CV_64F);

    // Tamaño de pixel (en metros/pixel). Usa valor absoluto por si la Y viene negativa.
    const double cellSize = mCellSize;

    // Gradientes (Sobel 3x3) y normalización por el tamaño de pixel:
    cv::Mat dzdx, dzdy;
    cv::Sobel(dem64, dzdx, CV_64F, 1, 0, 3);
    cv::Sobel(dem64, dzdy, CV_64F, 0, 1, 3);
    dzdx /= (8.0 * cellSize);
    dzdy /= (8.0 * cellSize);

    // Normal de la superficie: n = (-p, -q, 1) / sqrt(p^2 + q^2 + 1)
    cv::Mat p2, q2, denom, nx, ny, nz;
    cv::multiply(dzdx, dzdx, p2);
    cv::multiply(dzdy, dzdy, q2);
    cv::sqrt(p2 + q2 + 1.0, denom);
    nx = -dzdx / denom;
    ny = -dzdy / denom;
    cv::divide(1.0, denom, nz);

    // TODO: Se puede calcular antes
    // Vector solar: azimuth desde el norte (horario) y altitud
    double azimuth_rad = mAzimuth * tl::consts::deg_to_rad<double>;
    double altitud_rad = mAltitude * tl::consts::deg_to_rad<double>;
    double zen_rad = tl::consts::half_pi<double> -altitud_rad;

    // TODO: Se puede calcular antes
    // Si el eje Y de imagen crece hacia abajo, este mapeo cuadra con Sobel:
    double lx = std::sin(zen_rad) * std::sin(azimuth_rad); // E-O
    double ly = std::sin(zen_rad) * std::cos(azimuth_rad); // N-S (positivo hacia abajo)
    double lz = std::cos(zen_rad);

    // Hillshade lambertiano: shade = dot(n, l)
    cv::Mat shade = nx * lx + ny * ly + nz * lz;

    // Clamp fijo a [0,1] (¡¡NO normalizar por-tile!!)
    cv::max(shade, 0.0, shade);
    cv::min(shade, 1.0, shade);

    // (Opcional) luz ambiental/ganancia/gamma, siempre fijos para todas las teselas
    double ambient = 0.15; // 0..1
    double gain = 0.85;    // 0..1, tal que ambient+gain<=1
    shade = ambient + gain * shade;
    cv::min(shade, 1.0, shade);

    // 0..1 -> 0..255
    cv::Mat hillshade8u;
    shade.convertTo(hillshade8u, CV_8U, 255.0);

    // Salida: gris o color + sombreado
    cv::Mat output;
    if (mColorize) {
        // Colorizar el DEM con RANGO GLOBAL (no por-tile)
        cv::Mat dem8u;
        if (global_max > global_min) {
            double scale = 255.0 / (global_max - global_min);
            double shift = -global_min * scale;
            dem64.convertTo(dem8u, CV_8U, scale, shift);
        }

        cv::Mat demColor;
        cv::applyColorMap(dem8u, demColor, cv::COLORMAP_TURBO);

        cv::Mat hs3;
        cv::cvtColor(hillshade8u, hs3, cv::COLOR_GRAY2BGR);
        cv::multiply(demColor, hs3, output, 1.0 / 255.0);  // mezcla consistente entre tiles
    } else {
        output = hillshade8u;
    }

    // Devolver al buffer original     
    cv::Mat mask;
    if (mHasNoDataValue) {
        cv::inRange(mat_source, cv::Scalar::all(mNoDataValue), cv::Scalar::all(mNoDataValue), mask);
        cv::bitwise_not(mask, mask);
    } else {
        mask = cv::Mat(mat_source.size(), CV_8U, cv::Scalar(255));
    }

    output.copyTo(mat_source, mask);
}









}