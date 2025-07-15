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

#include "graphos/widgets/MapViewer.h"

#include "graphos/core/utils.h"

#include <tidop/math/geometry/affine.h>
#include <tidop/img/imgreader.h>
#include <tidop/geometry/size.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>

#include <QVBoxLayout>
#include <QFileInfo>
#include <QGeoView/QGVMap.h>
#include <QGeoView/QGVLayer.h>
#include <QGeoView/QGVLayerOSM.h>
#include <QGeoView/Raster/QGVImage.h>
#include <QGeoView/QGVLayerGoogle.h>
#include <QGeoView/QGVLayerBing.h>

#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <gdalwarper.h>

#include <QBrush>
#include <QPen>
#include <QTimer>
#include <QPainter>



QGVPolygon::QGVPolygon(const PointList &geoPoints, QColor stroke, QColor fill)
    : mGeoPoints(geoPoints)
    , mColorStroke(stroke)
    , mColorFill(fill)
{
}

void QGVPolygon::setPoints(const PointList &geoPoints)
{
    mGeoPoints = geoPoints;

    // Geo coordinates need to be converted manually again to projection
    onProjection(getMap());

    // Now we can inform QGV about changes for this
    resetBoundary();
    refresh();
}

PointList QGVPolygon::getPoints() const
{
    return mGeoPoints;
}

void QGVPolygon::onProjection(QGVMap *geoMap)
{
    QGVDrawItem::onProjection(geoMap);
    mProjPoints.clear();
    for (const QGV::GeoPos &pos : mGeoPoints)
        mProjPoints << geoMap->getProjection()->geoToProj(pos);
}

QPainterPath QGVPolygon::projShape() const
{
    QPainterPath path;
    path.addPolygon(mProjPoints);
    return path;
}

void QGVPolygon::projPaint(QPainter *painter)
{
    QPen pen = QPen(QBrush(mColorStroke), 1);
    pen.setCosmetic(true);
    painter->setPen(pen);
    painter->setBrush(QBrush(mColorFill));
    painter->drawPolygon(mProjPoints);
}

QPointF QGVPolygon::projAnchor() const
{
    return mProjPoints.boundingRect().center();
}

QTransform QGVPolygon::projTransform() const
{
    // This method is optional (needed flag is QGV::ItemFlag::Transformed).
    // Custom transformation for item.
    // In this case we rotate item by 45 degree.

    return QGV::createTransfromAzimuth(projAnchor(), 45);
}

QString QGVPolygon::projTooltip(const QPointF &projPos) const
{
    // This method is optional (when empty return then no tooltip).
    // Text for mouse tool tip.

    auto geo = getMap()->getProjection()->projToGeo(projPos);

    return "Polygon with color " + mColorFill.name() + "\nPosition " + geo.latToString() + " " + geo.lonToString();
}

void QGVPolygon::projOnMouseClick(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Clickable).
    // Custom reaction to item single mouse click.
    // To avoid collision with item selection this code applies only if item selection disabled.
    // In this case we change opacity for item.

    if (!isSelectable()) {
        if (getOpacity() <= 0.5)
            setOpacity(1.0);
        else
            setOpacity(0.5);

        qInfo() << "single click" << projPos;
    } else {
        setOpacity(1.0);
    }
}

void QGVPolygon::projOnMouseDoubleClick(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Clickable).
    // Custom reaction to item double mouse click.
    // In this case we change color for item.

    const QList<QColor> colors = {Qt::red, Qt::blue, Qt::green, Qt::gray, Qt::cyan, Qt::magenta, Qt::yellow};

    const auto iter =
        std::find_if(colors.begin(), colors.end(), [this](const QColor &color) { return color == mColorFill; });
    mColorFill = colors[(iter - colors.begin() + 1) % colors.size()];
    repaint();

    setOpacity(1.0);

    qInfo() << "double click" << projPos;
}

void QGVPolygon::projOnObjectStartMove(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move start.
    // In this case we only log message.

    qInfo() << "object move started at" << projPos;
}

void QGVPolygon::projOnObjectMovePos(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to mouse pos change when item move is started.
    // In this case actually changing location of object.

    PointList newPoints;
    for (const QPointF &pt : mProjPoints)
        newPoints << getMap()->getProjection()->projToGeo(pt + projPos);

    setPoints(newPoints);

    qInfo() << "object moved" << mProjPoints;
}

void QGVPolygon::projOnObjectStopMove(const QPointF &projPos)
{
    // This method is optional (needed flag is QGV::ItemFlag::Movable).
    // Custom reaction to item move finished.
    // In this case we only log message.

    qInfo() << "object move stopped" << projPos;
}





RasterTiledLayer::RasterTiledLayer(const QString &tifPath)
  : mTifPath(tifPath),
    mDataset(nullptr)
{
    GDALAllRegister();
    mDataset = static_cast<GDALDataset *>(GDALOpen(tifPath.toUtf8().data(), GA_ReadOnly));
    if (!mDataset) {
        qCritical() << "No se pudo abrir la imagen raster:" << tifPath;
        return;
    }

    // Extraer extensión geográfica del tif
    double geoTransform[6];
    if (mDataset->GetGeoTransform(geoTransform) == CE_None) {
        double originX = geoTransform[0];
        double originY = geoTransform[3];
        double pixelSizeX = geoTransform[1];
        double pixelSizeY = geoTransform[5];

        int width = mDataset->GetRasterXSize();
        int height = mDataset->GetRasterYSize();

        double maxX = originX + width * pixelSizeX;
        double minY = originY + height * pixelSizeY;

        OGRCoordinateTransformation *transform = nullptr;

        // Obtener CRS del GeoTiff
        const OGRSpatialReference *sourceRef = mDataset->GetSpatialRef();
        OGRSpatialReference targetRef;
        targetRef.importFromEPSG(4326);

        if (sourceRef && !sourceRef->IsSame(&targetRef)) {
            transform = OGRCreateCoordinateTransformation(sourceRef, &targetRef);

            transform->Transform(1, &originX, &originY);
            transform->Transform(1, &maxX, &minY);

            OCTDestroyCoordinateTransformation(transform);
        }

        mGeoExtent = QGV::GeoRect(originX, originY, maxX, minY);

    }

    setZValue(-1);

    // Reemplazar lo anterior por esto

    reader = tl::ImageReaderFactory::create(tl::Path(tifPath.toStdString()));
    reader->open();
}

RasterTiledLayer::~RasterTiledLayer()
{
    if (mDataset) {
        GDALClose(mDataset);
    }

    reader->close();
}

QGV::GeoRect RasterTiledLayer::maxGeoExtent() const
{
    return mGeoExtent;
}

int RasterTiledLayer::minZoomlevel() const { return 1; }
int RasterTiledLayer::maxZoomlevel() const { return 20; }

void RasterTiledLayer::onProjection(QGVMap *geoMap)
{
    QGVLayerTiles::onProjection(geoMap);

    // Limitar área activa a donde hay datos
    mTileGridBounds = QRect(
        QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.topLeft()).pos(),
        QGV::GeoTilePos::geoToTilePos(minZoomlevel(), mGeoExtent.bottomRight()).pos()
    );
}


void RasterTiledLayer::request(const QGV::GeoTilePos &tilePos)
{
    if (!mDataset) return;

    auto tilePosParent = tilePos.parent(minZoomlevel()).pos();
    if (!mTileGridBounds.contains(tilePosParent)) {
        return;
    }

    // Obtener rectángulo geográfico del tile
    QGV::GeoRect tileGeoRect = tilePos.toGeoRect();

    if (!mGeoExtent.intersects(tileGeoRect)) return;

    QRectF tile_proj_rect = getMap()->getProjection()->geoToProj(tileGeoRect);

    QRectF image_rect = getMap()->getProjection()->geoToProj(mGeoExtent);

    // Esto no parece tener mucho sentido aqui...
    std::array<double, 6> geoTransform{};
    if (mDataset->GetGeoTransform(geoTransform.data()) != CE_None) return;

    tl::Affine<double, 2> affine;
    affine(0, 0) = geoTransform[1];
    affine(0, 1) = geoTransform[2];
    affine(0, 2) = geoTransform[0];
    affine(1, 0) = geoTransform[4];
    affine(1, 1) = geoTransform[5];
    affine(1, 2) = geoTransform[3];


    tl::GeoTools *ptrGeoTools = tl::GeoTools::getInstance();
    OGRCoordinateTransformation *transform_to_raster = nullptr;
    OGRCoordinateTransformation *transform_to_3857 = nullptr;
    OGRSpatialReference targetRef;
    // Comprobar si hay cambio de CRS
    {
        // Obtener CRS del shapefile
        const OGRSpatialReference *sourceRef = mDataset->GetSpatialRef();
        if (sourceRef) { // Si no tiene CRS tendría que devolver un error

            const char *auth_name = sourceRef->GetAuthorityName(nullptr);
            const char *auth_code = sourceRef->GetAuthorityCode(nullptr);

            if (auth_name && std::string(auth_name) == "EPSG" && auth_code) {

                int src_epsg = std::stoi(auth_code);
                int dst_epsg = 3857;

                if (src_epsg != dst_epsg) {

                    targetRef.importFromEPSG(4326);

                    transform_to_raster = OGRCreateCoordinateTransformation(sourceRef, &targetRef);
                    //transform_to_3857 = OGRCreateCoordinateTransformation(&targetRef, sourceRef);

                }
            }
        }
    }

    int tileSize = 256; // Tamaño del tile en píxeles
    int tile_size_x = tileSize;
    int tile_size_y = tileSize;

    if (transform_to_raster) {

        // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
        // la Y sale con el signo cambiado
        tl::Point3d pt1(tile_proj_rect.left(), -tile_proj_rect.top(), 0.);
        tl::Point3d pt2(tile_proj_rect.right(), -tile_proj_rect.bottom(), 0.);

        //double x1 = tile_proj_rect.left();
        //double y1 = tile_proj_rect.top();
        //double x2 = tile_proj_rect.right();
        //double y2 = tile_proj_rect.bottom();

        //TL_ASSERT(transform_to_3857->Transform(1, &x1, &y1) && transform_to_3857->Transform(1, &x2, &y2), "");
        pt1 = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", "EPSG:25830", pt1);
        pt2 = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", "EPSG:25830", pt2);

        auto p1 = affine.inverse().transform(static_cast<tl::Point2d>(pt1));
        auto p2 = affine.inverse().transform(static_cast<tl::Point2d>(pt2));

        int xOffset = std::floor(std::min(p1.x, p2.x));
        int yOffset = std::floor(std::min(p1.y, p2.y));
        int xSize = std::ceil(std::abs(p2.x - p1.x));
        int ySize = std::ceil(std::abs(p2.y - p1.y));

        double sx = tile_size_x / static_cast<double>(xSize);
        double sy = tile_size_y / static_cast<double>(ySize);

        tl::Rect<int> rect(xOffset, yOffset, xSize, ySize);
        tl::Affine<int, 2> affine_out;
        cv::Mat mat1 = reader->read(sx, sy, rect, &affine_out);


        // 1. Crear dataset MEM temporal de origen
        GDALDriver *memDriver = GetGDALDriverManager()->GetDriverByName("MEM");
        if (memDriver == nullptr) {
            std::cerr << "Error: El driver 'MEM' no está disponible." << std::endl;
            return;
        }

        GDALDataset *srcDS = memDriver->Create("", xSize, ySize, 3 /* Número de bandas */, GDT_Byte, nullptr);
        if (srcDS == nullptr) {
            std::cerr << "Error: No se pudo crear el dataset en memoria para la región." << std::endl;
            return;
        }

        // 3. Calcular las coordenadas geográficas de la esquina superior izquierda de la región
        double dfOriginXRegion = geoTransform[0] + affine_out.translation().x() * geoTransform[1] + affine_out.translation().y() * geoTransform[2];
        double dfOriginYRegion = geoTransform[3] + affine_out.translation().x() * geoTransform[4] + affine_out.translation().y() * geoTransform[5];

        double adfGeoTransformRegion[6];
        adfGeoTransformRegion[0] = dfOriginXRegion;
        adfGeoTransformRegion[1] = geoTransform[1];
        adfGeoTransformRegion[2] = geoTransform[2];
        adfGeoTransformRegion[3] = dfOriginYRegion;
        adfGeoTransformRegion[4] = geoTransform[4];
        adfGeoTransformRegion[5] = geoTransform[5];

        srcDS->SetGeoTransform(adfGeoTransformRegion);
        srcDS->SetProjection(mDataset->GetProjectionRef());

        std::vector<int> band_order{3, 2, 1};
        srcDS->RasterIO(GF_Write, 0, 0, mat1.cols, mat1.rows, 
            mat1.ptr(), mat1.cols, mat1.rows,
            GDT_Byte, 3, band_order.data(), 3, 3 * mat1.cols, 1);

        //// Leer desde disco
        //mDataset->RasterIO(GF_Read, xOffset, yOffset, xSize, ySize,
        //    srcDS->GetRasterBand(1)->GetWritableBlockRef(0, 0), // Esto se lo ha inventado
        //    xSize, ySize, GDT_Byte, 3, nullptr,
        //    3, xSize * 3, 1);

        // 6. Definir el nuevo sistema de referencia de coordenadas de destino
        //OGRSpatialReference oTargetSRS;
        //oTargetSRS.importFromEPSG(nTargetEPSG);
        //char *pszTargetWKT = nullptr;
        //oTargetSRS.exportToWkt(&pszTargetWKT);

        //if (pszTargetWKT == nullptr) {
        //    std::cerr << "Error: No se pudo exportar el WKT del CRS de destino." << std::endl;
        //    return;
        //}

        GDALDataset *warpedDS = static_cast<GDALDataset *>(GDALAutoCreateWarpedVRT(srcDS,
                                                                                   mDataset->GetProjectionRef(),  // fuente WKT
                                                                                   "EPSG:3857",                   // destino 
                                                                                    GRA_Bilinear, 0.0, nullptr));

        TL_ASSERT(warpedDS != nullptr, "Failed to reproject: {}", CPLGetLastErrorMsg());


        // Ahora creamos un dataset final en memoria para escribir los datos reproyectados
        // Necesitamos obtener las dimensiones finales del VRT
        int nDstWidth = warpedDS->GetRasterXSize();
        int nDstHeight = warpedDS->GetRasterYSize();

        GDALDataset *poDstMemDS = memDriver->Create("", nDstWidth, nDstHeight, 3/*nBands*/, GDT_Byte/*eDataType*/, nullptr);
        if (poDstMemDS == nullptr) {
            std::cerr << "Error: No se pudo crear el dataset de destino en memoria." << std::endl;
            //CPLFree(pszTargetWKT);
            //GDALClose(warpedDS);
            //GDALClose(srcDS);
            //GDALClose(poSrcDS);
            return;
        }

        // Asignar el CRS y geotransformación del VRT al dataset final en memoria
        double geoTransform[6];
        if (warpedDS->GetGeoTransform(geoTransform) == CE_None) {
            poDstMemDS->SetGeoTransform(geoTransform);
        }
        poDstMemDS->SetProjection(warpedDS->GetProjectionRef());

        // 8. Realizar la reproyección real de datos
    // GDALWarpOptions (GDAL 2.x+) es la forma moderna de configurar GDALWarp
        GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();
        psWarpOptions->papszWarpOptions = CSLAddNameValue(psWarpOptions->papszWarpOptions, "NUM_THREADS", "ALL_CPUS");
        psWarpOptions->eResampleAlg = GRA_Bilinear; // Ajusta según necesites (GRA_Bilinear, GRA_Cubic)
        //psWarpOptions->papszSrcSRS = CSLAddString(nullptr, pszProjectionOriginalWKT);
        //psWarpOptions->papszDstSRS = CSLAddString(nullptr, pszTargetWKT);
        psWarpOptions->hSrcDS = srcDS;
        psWarpOptions->hDstDS = poDstMemDS;
        // psWarpOptions->pfnProgress = GDALTermProgress; // Para ver el progreso en consola

        GDALProgressFunc pfnProgress = GDALTermProgress; // Puedes usar nullptr para no progreso
        void *pProgressArg = nullptr; // Puedes usar nullptr si no usas pfnProgress

        GDALWarpOperation oWarp;
        CPLErr eWarpErr = oWarp.Initialize(psWarpOptions);
        if (eWarpErr != CE_None) {
            std::cerr << "Error: Falló la inicialización de GDALWarp." << std::endl;
            GDALDestroyWarpOptions(psWarpOptions);
            //CPLFree(pszTargetWKT);
            GDALClose(poDstMemDS);
            GDALClose(warpedDS);
            GDALClose(srcDS);
            //GDALClose(poSrcDS);
            return;
        }

        eWarpErr = oWarp.ChunkAndWarpImage(0, 0, nDstWidth, nDstHeight);
        if (eWarpErr != CE_None) {
            std::cerr << "Error: Falló la operación de GDALWarp." << std::endl;
            GDALDestroyWarpOptions(psWarpOptions);
            //CPLFree(pszTargetWKT);
            GDALClose(poDstMemDS);
            GDALClose(warpedDS);
            //GDALClose(poSrcMemDS);
            //GDALClose(poSrcDS);
            return;
        }

        GDALDestroyWarpOptions(psWarpOptions);
        //CPLFree(pszTargetWKT); // Liberar el WKT generado

        // 9. Leer los datos del dataset reproyectado en memoria
        result.nWidth = nDstWidth;
        result.nHeight = nDstHeight;
        result.nBands = nBands;
        result.eDataType = eDataType;

        for (int i = 0; i < 6; ++i) {
            result.adfGeoTransform[i] = poDstMemDS->GetGeoTransform()[i];
        }
        result.pszProjectionWKT = CPLStrdup(poDstMemDS->GetProjectionRef()); // Copiar el WKT

        size_t nTotalPixels = (size_t)nDstWidth * nDstHeight * nBands;
        result.data.resize(nTotalPixels * nPixelSize); // Ajustar el tamaño del vector

        // Leer todas las bandas
        for (int i = 0; i < nBands; ++i) {
            GDALRasterBand *poBandDst = poDstMemDS->GetRasterBand(i + 1);
            CPLErr eReadErr = poBandDst->ReadRaster(0, 0, nDstWidth, nDstHeight,
                result.data.data() + (i * (size_t)nDstWidth * nDstHeight * nPixelSize),
                nDstWidth, nDstHeight, eDataType, 0, 0);
            if (eReadErr != CE_None) {
                std::cerr << "Error: Falló la lectura de la banda " << i + 1 << " del dataset de destino." << std::endl;
                GDALClose(poDstMemDS);
                GDALClose(warpedDS);
                GDALClose(srcDS);
                GDALClose(poSrcDS);
                return TransformedRegion(); // Devuelve un objeto vacío
            }
        }

        // 10. Liberar los recursos de GDAL
        GDALClose(poDstMemDS); // Cierra el dataset de destino en memoria
        GDALClose(warpedDS); // Cierra el VRT
        GDALClose(poSrcMemDS); // Cierra el dataset de origen en memoria
        GDALClose(poSrcDS);    // Cierra el dataset original



        OCTDestroyCoordinateTransformation(transform_to_3857);

        return;
    }

    // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
    // la Y sale con el signo cambiado
    auto p1 = affine.inverse().transform({tile_proj_rect.left(), -tile_proj_rect.top()}); 
    auto p2 = affine.inverse().transform({tile_proj_rect.right(), -tile_proj_rect.bottom()});

    int xOffset = std::floor(std::min(p1.x, p2.x));
    int yOffset = std::floor(std::min(p1.y, p2.y));
    int xSize = std::ceil(std::abs(p2.x - p1.x));
    int ySize = std::ceil(std::abs(p2.y - p1.y));


    double sx = tile_size_x / static_cast<double>(xSize);
    double sy = tile_size_y / static_cast<double>(ySize);

    // Con esto se lee la imagen y en affine_out esta el offset
    //tl::Rect<int> rect(xOffset, yOffset, xSize, ySize);
    //tl::Affine<int, 2> affine_out;
    //cv::Mat mat1 = reader->read(sx, sy, rect, &affine_out);

    int tile_offset_x = 0;
    int tile_offset_y = 0;
    auto rows = mDataset->GetRasterYSize();
    auto cols = mDataset->GetRasterXSize();
    if (xOffset < 0) {
        xSize = xSize + xOffset;
        xOffset = 0;
        tile_size_x = sx * xSize;
        tile_offset_x = tileSize - tile_size_x;
    }
    if (yOffset < 0) {
        ySize = ySize + yOffset;
        yOffset = 0;
        tile_size_y = sy * ySize;
        tile_offset_y = tileSize - tile_size_y;
    }

    if (xOffset + xSize > cols) {
        xSize = cols - xOffset;
        tile_size_x = sx * xSize;
    }

    if (yOffset + ySize > rows) {
        ySize = rows - yOffset;
        tile_size_y = sy * ySize;
    }

    //tl::Window<tl::Point<double>> window_proj_tile(tl::Point<double>(tile_proj_rect.left(), -tile_proj_rect.top()),
    //                                               tl::Point<double>(tile_proj_rect.right(), -tile_proj_rect.bottom()));
    //window_proj_tile.normalized();
    //tl::Window<tl::Point<double>> window_image(tl::Point<double>(image_rect.left(), -image_rect.top()),
    //                                           tl::Point<double>(image_rect.right(), -image_rect.bottom()));
    //window_image.normalized();
    //
    //auto window = tl::windowIntersection(window_proj_tile, window_image);

    //double scale_x = window.width() / 256.;
    //double scale_y = window.height() / 256.;


    // Calcular tamaño en píxeles del tile
    QImage tileImage(tileSize, tileSize, QImage::Format_RGBA8888);
    tileImage.fill(Qt::transparent);
    
    uint8_t *buffer = new uint8_t[tile_size_x * tile_size_y * 3];

    CPLErr err = mDataset->RasterIO(GF_Read,
                                    xOffset, yOffset, xSize, ySize,
                                    buffer, tile_size_x, tile_size_y,
                                    GDT_Byte, 3, nullptr,
                                    3, tile_size_x * 3, 1);
    if (err == CE_None) {
        for (int y = 0; y < tile_size_y; ++y) {
            for (int x = 0; x < tile_size_x; ++x) {
                int i = y * tile_size_x + x;
                uint8_t r = buffer[i * 3 + 0];
                uint8_t g = buffer[i * 3 + 1];
                uint8_t b = buffer[i * 3 + 2];
                int alpha = 255;
                if (r == 0 && g == 0 && b == 0)
                    alpha = 0;
                tileImage.setPixelColor(tile_offset_x + x, tile_offset_y + y, QColor(r, g, b, alpha));
            }
        }
    } else { return;  }

    if (tileImage.isNull()) {
        qWarning() << "No se pudo cargar la imagen del tile:" << tilePos.pos();
        delete[] buffer;
        return;
    }

    //cv::Mat mat2 = graphos::qImageToCvMat(tileImage);

    auto *imageItem = new QGVImage();
    imageItem->setGeometry(tile_proj_rect);
    imageItem->loadImage(tileImage);
    imageItem->setCeilingOnScale(false);
    imageItem->setSelectable(false);
    onTile(tilePos, imageItem);

    delete[] buffer;
}

void RasterTiledLayer::cancel(const QGV::GeoTilePos & /*tilePos*/)
{
    // No es necesario si no usas hilos o peticiones asíncronas
}

QList<QGV::GeoPos> convert(OGRPolygon *poPolygon)
{
    QList<QGV::GeoPos> result;
    OGRLinearRing *ring = poPolygon->getExteriorRing();
    for (int i = 0; i < ring->getNumPoints(); ++i) {
        OGRPoint pt;
        ring->getPoint(i, &pt);
        result << QGV::GeoPos(pt.getX(), pt.getY());  // lat, lon
    }
    return result;
}

namespace graphos
{

MapViewer::MapViewer(QWidget *parent)
  : QWidget(parent)
{
    mMap = new QGVMap(this);

    QDir("cacheDir").removeRecursively();
    auto cache = new QNetworkDiskCache(parent);
    cache->setCacheDirectory("cacheDir");
    auto manager = new QNetworkAccessManager(parent);
    manager->setCache(cache);
    QGV::setNetworkManager(manager);

    // Background layer
    //auto osmLayer = new QGVLayerOSM();
    //auto googleLayer = new QGVLayerGoogle(QGV::TilesType::Satellite);
    auto bingLayer = new QGVLayerBing(QGV::TilesType::Satellite);

    mMap->addItem(bingLayer);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mMap);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

MapViewer::~MapViewer()
{
}

//void MapViewer::setProjection(const QString &epsgCode)
//{
//    mMap->setCrs(epsgCode);
//}

void MapViewer::loadImage(const QString &imagePath)
{
    //auto *imageLayer = new QGVImageLayer();
    //if (!imageLayer->setSource(imagePath)) {
    //    qWarning("No se pudo cargar la ortofoto: %s", qUtf8Printable(imagePath));
    //    delete imageLayer;
    //    return;
    //}

    //imageLayer->setName(QFileInfo(imagePath).fileName());
    //mMap->addLayer(imageLayer);
}

void MapViewer::loadShapefile(const QString &shapefilePath)
{
    GDALAllRegister();

    GDALDataset *poDS = static_cast<GDALDataset *>(
        GDALOpenEx(shapefilePath.toUtf8().data(), GDAL_OF_VECTOR, NULL, NULL, NULL));
    if (poDS == NULL) {
        qWarning() << "Failed to open shapefile:" << shapefilePath;
        return;
    }

    OGRCoordinateTransformation *transform = nullptr;

    // Obtener CRS del shapefile
    OGRSpatialReference *sourceRef = poDS->GetLayer(0)->GetSpatialRef();
    OGRSpatialReference targetRef;
    //targetRef.importFromEPSG(3857); // EPSG:3857
    targetRef.importFromEPSG(4326);

    if (sourceRef && !sourceRef->IsSame(&targetRef)) {
        transform = OGRCreateCoordinateTransformation(sourceRef, &targetRef);
    }

    for (int iLayer = 0; iLayer < poDS->GetLayerCount(); iLayer++) {
        OGRLayer *poLayer = poDS->GetLayer(iLayer);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
            OGRGeometry *poGeometry = poFeature->GetGeometryRef();
            if (!poGeometry) continue;

            if (transform) {
                poGeometry->transform(transform);
            }
            OGREnvelope envelope;
            poGeometry->getEnvelope(&envelope);
            mVectorExtent |= QRectF(envelope.MinX, envelope.MinY, envelope.MaxX - envelope.MinX,
                                    envelope.MaxY - envelope.MinY);

            const auto type = wkbFlatten(poGeometry->getGeometryType());

            if (type == wkbPolygon) {
                OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
                //if (poPolygon->IsValid()) {
                    QList<QGV::GeoPos> points = convert(poPolygon);
                    if (points.size() > 2) {
                        mMap->addItem(new QGVPolygon(points, Qt::black, Qt::cyan));
                    }
                //}
            } else if (type == wkbMultiPolygon) {
                OGRMultiPolygon *poMultiPolygon = (OGRMultiPolygon *)poGeometry;
                for (int i = 0; i < poMultiPolygon->getNumGeometries(); ++i) {
                    OGRPolygon *poPolygon = (OGRPolygon *)poMultiPolygon->getGeometryRef(i);
                    //if (poPolygon->IsValid()) {
                        QList<QGV::GeoPos> points = convert(poPolygon);
                        if (points.size() > 2) {
                            mMap->addItem(new QGVPolygon(points, Qt::black, Qt::green));
                        }
                    //}
                }
            }

            OGRFeature::DestroyFeature(poFeature);
        }
    }

    GDALClose(poDS);
    if (transform) OCTDestroyCoordinateTransformation(transform);


    //mMap->cameraTo(QGVCameraActions(mMap).scaleTo(vectorExtent));

    // Ajustar vista de cámara
    //QTimer::singleShot(100, this, [this]() {
    //    auto target = maxGeoExtent();// mMap->getProjection()->boundaryGeoRect();
    //        mMap->cameraTo(QGVCameraActions(mMap).scaleTo(target));
    //});
}

void MapViewer::loadGeoTiff(const QString &tifPath)
{
    auto layer = new RasterTiledLayer(tifPath);
    mMap->addItem(layer);
    //mMap->setProjection(QGV::Projection::EPSG3857);
    //mMap->refreshProjection();
    auto max_geo_extend = layer->maxGeoExtent();
    max_geo_extend.topLeft();
    mVectorExtent |= QRectF(QPointF(max_geo_extend.topLeft().latitude(), max_geo_extend.topLeft().longitude()), 
                            QPointF(max_geo_extend.bottomRight().latitude(), max_geo_extend.bottomRight().longitude()));
}

void MapViewer::zoomExtend() const
{
    auto target = maxGeoExtent();
    mMap->cameraTo(QGVCameraActions(mMap).scaleTo(target));
}

QGV::GeoRect MapViewer::maxGeoExtent() const
{
    return QGV::GeoRect(QGV::GeoPos(mVectorExtent.topLeft().x(), mVectorExtent.topLeft().y()),
                        QGV::GeoPos(mVectorExtent.bottomRight().x(), mVectorExtent.bottomRight().y()));
}


}