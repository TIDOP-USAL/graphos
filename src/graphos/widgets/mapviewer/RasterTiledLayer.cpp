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
#include <tidop/img/imgreader.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geometry/algorithms/distance.h>
#include <tidop/math/geometry/affine.h>

/* QGeoView */
#include <QGeoView/Raster/QGVImage.h>

/* GDAL */
#include <gdal.h>
#include <gdal_priv.h>
#include <gdalwarper.h>



namespace graphos
{

RasterTiledLayer::RasterTiledLayer(const QString &tifPath, bool dem)
  : mDEM(dem)
{
    setZValue(-1);

    reader = tl::ImageReaderFactory::create(tl::Path(tifPath.toStdString()));
    reader->open();
    auto georeference = reader->georeference();
    mAffineImageToWorld = georeference.inverse();
    auto window = reader->window();

    OGRCoordinateTransformation *transform = nullptr;

    // Obtener CRS del GeoTiff
    //const OGRSpatialReference *sourceRef = mDataset->GetSpatialRef();
    OGRSpatialReference sourceRef;
    sourceRef.importFromWkt(reader->crsWkt().c_str());
    OGRSpatialReference targetRef;
    targetRef.importFromEPSG(4326);

    if (!sourceRef.IsEmpty() && !sourceRef.IsSame(&targetRef)) {
        transform = OGRCreateCoordinateTransformation(&sourceRef, &targetRef);

        transform->Transform(1, &window.pt1.x, &window.pt1.y);
        transform->Transform(1, &window.pt2.x, &window.pt2.y);

        OCTDestroyCoordinateTransformation(transform);

        const char *auth_name = sourceRef.GetAuthorityName(nullptr);
        const char *auth_code = sourceRef.GetAuthorityCode(nullptr);
        if (auth_name && std::string(auth_name) == "EPSG" && auth_code) {
            mEpsgSource = std::string(auth_name).append(":").append(auth_code);
            mCrsTransform = true;
        }
    }

    mGeoExtent = QGV::GeoRect(window.pt1.x, window.pt1.y, window.pt2.x, window.pt2.y);
    
    // Calculo de la escala máxima
    {
        auto rows = reader->rows();
        auto cols = reader->cols();
        auto p1 = mAffineImageToWorld.transform(tl::Point2d(0., 0.));
        auto p2 = mAffineImageToWorld.transform(tl::Point2d(cols, rows));
        
        mGSD = std::min(cols / std::abs(p1.x - p2.x), rows / std::abs(p1.y -p2.y));
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

    // Limitar área activa a donde hay datos
    mTileGridBounds = QRect(QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.topLeft()).pos(),
                            QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.bottomRight()).pos());
}



void RasterTiledLayer::request(const QGV::GeoTilePos &tilePos)
{
    try {

        if (!reader->isOpen()) return;

        auto tilePosParent = tilePos.parent(minZoomlevel()).pos();
        if (!mTileGridBounds.contains(tilePosParent)) {
            return;
        }

        // Obtener rectángulo geográfico del tile
        QGV::GeoRect tileGeoRect = tilePos.toGeoRect();

        if (!mGeoExtent.intersects(tileGeoRect)) return;

        // Rectangulo del tile en coordenadas proyectadas
        QRectF tile_proj_rect = getMap()->getProjection()->geoToProj(tileGeoRect);

        // Rectangulo de la imagen en coordenadas proyectadas
        QRectF rect_image_epsg_3857 = getMap()->getProjection()->geoToProj(mGeoExtent);

        tl::GeoTools *ptrGeoTools = tl::GeoTools::getInstance();

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
            std::vector<tl::Point<double>> points_tile(4);
            points_tile[0] = tl::Point<double>(tile_proj_rect.left(), -tile_proj_rect.top());
            points_tile[1] = tl::Point<double>(tile_proj_rect.right(), -tile_proj_rect.top());
            points_tile[2] = tl::Point<double>(tile_proj_rect.right(), -tile_proj_rect.bottom());
            points_tile[3] = tl::Point<double>(tile_proj_rect.left(), -tile_proj_rect.bottom());

            std::vector<tl::Point<double>> points_image_epsg_3857(4);
            points_image_epsg_3857[0] = tl::Point<double>(rect_image_epsg_3857.left(), -rect_image_epsg_3857.top());
            points_image_epsg_3857[1] = tl::Point<double>(rect_image_epsg_3857.right(), -rect_image_epsg_3857.top());
            points_image_epsg_3857[2] = tl::Point<double>(rect_image_epsg_3857.right(), -rect_image_epsg_3857.bottom());
            points_image_epsg_3857[3] = tl::Point<double>(rect_image_epsg_3857.left(), -rect_image_epsg_3857.bottom());
            

            // 1 - Tile window
            std::vector<tl::Point<double>> tile_window(4);
            tile_window[0] = tl::Point<double>(0., 0.);
            tile_window[1] = tl::Point<double>(tileSize, 0.);
            tile_window[2] = tl::Point<double>(tileSize, tileSize);
            tile_window[3] = tl::Point<double>(0., tileSize);

            // 2 - Affine transformation
            auto affine_image_epsg_3857 = tl::Affine2DEstimator<double>::estimate(tile_window, points_tile);

            // 3 - Ventana intersección
            tl::WindowD window_tile_epsg_3857(points_tile);
            tl::WindowD window_image_epsg_3857(points_image_epsg_3857);
            tl::WindowD window_read_3857 = tl::windowIntersection(window_tile_epsg_3857, window_image_epsg_3857);

            // 4 - Con la ventana intersección y la transformación afin calcular la zona con datos dentro de la tesela
            tl::Window<tl::Point<int>> window_image_to_read_3857;
            window_image_to_read_3857.pt1 = static_cast<tl::Point<int>>(affine_image_epsg_3857.inverse().transform(window_read_3857.pt1));
            window_image_to_read_3857.pt2 = affine_image_epsg_3857.inverse().transform(window_read_3857.pt2);
            window_image_to_read_3857.normalized();

            tl::Point<int> offset = window_image_to_read_3857.pt1;
            int width = window_image_to_read_3857.width()/* - offset.x*/;
            int height = window_image_to_read_3857.height()/* - offset.y*/;

            //5 - Calcular ventana a leer en coordenadas origen.

            std::vector<tl::Point<double>> points_source(4);
            points_source[0] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", mEpsgSource, tl::Point<double>(window_read_3857.pt1.x, window_read_3857.pt2.y));
            points_source[1] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", mEpsgSource, tl::Point<double>(window_read_3857.pt2.x, window_read_3857.pt2.y));
            points_source[2] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", mEpsgSource, tl::Point<double>(window_read_3857.pt2.x, window_read_3857.pt1.y));
            points_source[3] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", mEpsgSource, tl::Point<double>(window_read_3857.pt1.x, window_read_3857.pt1.y));

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


            tl::WindowD source_window(points_source);

            // Hay que expandir la ventana para evitar problemas en los bordes
            auto p1 = mAffineImageToWorld.transform(tl::Point2d(source_window.pt1.x, source_window.pt2.y));
            auto p2 = mAffineImageToWorld.transform(tl::Point2d(source_window.pt2.x, source_window.pt1.y));

            tl::Rect<int> rect_src(p1, p2);

            double xSize = rect_src.width;
            double ySize = rect_src.height;

            tl::Rect<int> rect_full_image(0, 0, reader->cols(), reader->rows());
            tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_src);

            if (rect_to_read.width == 0 || rect_to_read.height == 0) return;

            cv::Mat mat_source = reader->read(scale_src, scale_src, rect_to_read);

            if (mat_source.empty()) return;

            cv::Mat gray;
            if (mat_source.channels() == 3) {
                cv::cvtColor(mat_source, gray, cv::COLOR_BGR2GRAY);
            } else {
                gray = mat_source.clone();
            }

            // Cuenta cuántos píxeles son distintos de 0
            int nonZeroCount = cv::countNonZero(gray);
            gray.release();
            if (nonZeroCount == 0) return;
            
            if (mDEM) {

                buildDEM(mat_source);

            } else if (mat_source.channels() == 1) {

                cv::Mat temp;
                bool exist_nodata = false;
                double nodata_value = reader->noDataValue(&exist_nodata);
                if (!exist_nodata) nodata_value = tl::NoData<float>;
                cv::Mat mask;
                cv::inRange(mat_source, cv::Scalar::all(nodata_value), cv::Scalar::all(nodata_value), mask);
                cv::bitwise_not(mask, mask);

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
            tl::Point2d rectToReadTopLeft = rect_to_read.topLeft();

            tl::Point2d p0_rel = (src_p0_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p1_rel = (src_p1_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p2_rel = (src_p2_full - rectToReadTopLeft) * scale_src;
            tl::Point2d p3_rel = (src_p3_full - rectToReadTopLeft) * scale_src;

            // Ahora construimos src_points_img con esos puntos (flotantes)
            src_points_img[0] = cv::Point2f(static_cast<float>(p0_rel.x), static_cast<float>(p0_rel.y));
            src_points_img[1] = cv::Point2f(static_cast<float>(p1_rel.x), static_cast<float>(p1_rel.y));
            src_points_img[2] = cv::Point2f(static_cast<float>(p2_rel.x), static_cast<float>(p2_rel.y));
            src_points_img[3] = cv::Point2f(static_cast<float>(p3_rel.x), static_cast<float>(p3_rel.y));

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
            for (int y = 0; y < tileMat.rows; ++y) {
                for (int x = 0; x < tileMat.cols; ++x) {
                    int i = y * tileMat.cols + x;
                    uint8_t b = tileMat.data[i * 3 + 0];
                    uint8_t g = tileMat.data[i * 3 + 1];
                    uint8_t r = tileMat.data[i * 3 + 2];
                    int alpha = 255;
                    if (r == 0 && g == 0 && b == 0)
                        alpha = 0;
                    tileImage.setPixelColor(x + offset.x, y + offset.y, QColor(r, g, b, alpha));
                }
            }

        } else {

            // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
            // la Y sale con el signo cambiado
            auto p1 = mAffineImageToWorld.transform({tile_proj_rect.left(), -tile_proj_rect.top()});
            auto p2 = mAffineImageToWorld.transform({tile_proj_rect.right(), -tile_proj_rect.bottom()});

            int xOffset = std::floor(std::min(p1.x, p2.x));
            int yOffset = std::floor(std::min(p1.y, p2.y));
            int xSize = std::ceil(std::abs(p2.x - p1.x));
            int ySize = std::ceil(std::abs(p2.y - p1.y));

            double sx = tile_size_x_dst / static_cast<double>(xSize);
            double sy = tile_size_y_dst / static_cast<double>(ySize);

            int tile_offset_x = 0;
            int tile_offset_y = 0;
            auto rows = reader->rows();
            auto cols = reader->cols();
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
            cv::Mat mat_src = reader->read(rect_to_read, size_to_read);

            if (mat_src.empty()) return;

            // Calcular tamaño en píxeles del tile

            for (int y = 0; y < tile_size_y_dst; ++y) {
                for (int x = 0; x < tile_size_x_dst; ++x) {
                    int i = y * tile_size_x_dst + x;
                    uint8_t r = mat_src.data[i * 3 + 0];
                    uint8_t g = mat_src.data[i * 3 + 1];
                    uint8_t b = mat_src.data[i * 3 + 2];
                    int alpha = 255;
                    if (r == 0 && g == 0 && b == 0)
                        alpha = 0;
                    tileImage.setPixelColor(tile_offset_x + x, tile_offset_y + y, QColor(r, g, b, alpha));
                }
            }

        }


        if (!tileImage.isNull()) {

            tileImage.setDevicePixelRatio(2.);

            auto tile = new QGVImage();
            tile->setGeometry(tilePos.toGeoRect());
            tile->loadImage(tileImage);
            tile->setSelectable(false);
            onTile(tilePos, tile);
        }

    } catch (const std::exception &e) {
        tl::printException(e);
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
    const double cellSize = std::abs(reader->georeference().scale().x());

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
    bool exist_nodata = false;
    double nodata_value = reader->noDataValue(&exist_nodata);
    if (!exist_nodata) nodata_value = tl::NoData<float>;
    cv::Mat mask;
    cv::inRange(mat_source, cv::Scalar::all(nodata_value), cv::Scalar::all(nodata_value), mask);
    cv::bitwise_not(mask, mask);

    output.copyTo(mat_source, mask);
}

void RasterTiledLayer::cancel(const QGV::GeoTilePos &/*tilePos*/)
{
    qt_noop();
}







}