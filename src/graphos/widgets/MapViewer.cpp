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

#include <tidop/img/imgreader.h>
#include <tidop/geometry/size.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>
#include <tidop/geotools/CRSsTools.h>
#include <tidop/geotools/GeoTools.h>
#include <tidop/geometry/algorithms/distance.h>

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

GDALDataType GetGDALDataTypeFromBand(GDALRasterBand *poBand)
{
    if (poBand) {
        return poBand->GetRasterDataType();
    }
    return GDT_Unknown;
}

 // Estructura para almacenar la información de la región transformada
struct TransformedRegion
{
    std::vector<GByte> data; // Usamos GByte como ejemplo, adapta al tipo de dato real
    int nWidth;
    int nHeight;
    int nBands;
    GDALDataType eDataType;
    double adfGeoTransform[6];
    char *pszProjectionWKT; // WKT de la proyección, debe ser liberado con CPLFree

    TransformedRegion() : nWidth(0), nHeight(0), nBands(0), eDataType(GDT_Unknown), pszProjectionWKT(nullptr)
    {
        for (int i = 0; i < 6; ++i) adfGeoTransform[i] = 0.0;
    }

    // Constructor de movimiento
    TransformedRegion(TransformedRegion &&other) noexcept
        : data(std::move(other.data)),
        nWidth(other.nWidth),
        nHeight(other.nHeight),
        nBands(other.nBands),
        eDataType(other.eDataType),
        pszProjectionWKT(other.pszProjectionWKT)
    {
        for (int i = 0; i < 6; ++i) adfGeoTransform[i] = other.adfGeoTransform[i];
        other.pszProjectionWKT = nullptr; // Evitar doble free
    }

    // Operador de asignación de movimiento
    TransformedRegion &operator=(TransformedRegion &&other) noexcept
    {
        if (this != &other) {
            data = std::move(other.data);
            nWidth = other.nWidth;
            nHeight = other.nHeight;
            nBands = other.nBands;
            eDataType = other.eDataType;
            for (int i = 0; i < 6; ++i) adfGeoTransform[i] = other.adfGeoTransform[i];

            CPLFree(pszProjectionWKT); // Liberar el recurso actual
            pszProjectionWKT = other.pszProjectionWKT;
            other.pszProjectionWKT = nullptr;
        }
        return *this;
    }

    // Destructor para liberar la memoria de pszProjectionWKT
    ~TransformedRegion()
    {
        if (pszProjectionWKT) {
            CPLFree(pszProjectionWKT);
            pszProjectionWKT = nullptr;
        }
    }
};

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

        
        mGeoreference(0, 0) = geoTransform[1];
        mGeoreference(0, 1) = geoTransform[2];
        mGeoreference(0, 2) = geoTransform[0];
        mGeoreference(1, 0) = geoTransform[4];
        mGeoreference(1, 1) = geoTransform[5];
        mGeoreference(1, 2) = geoTransform[3];
    }

    setZValue(-1);

    // Reemplazar lo anterior por esto

    reader = tl::ImageReaderFactory::create(tl::Path(tifPath.toStdString()));
    reader->open();

    // Calculo de la escala máxima
    {
        auto rows = reader->rows();
        auto cols = reader->cols();
        auto p1 = mGeoreference.inverse().transform(tl::Point2d(0., 0.));
        auto p2 = mGeoreference.inverse().transform(tl::Point2d(cols, rows));
        
        mScaleMin = std::min(cols / std::abs(p1.x - p2.x), rows / std::abs(p1.y -p2.y));
    }
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

int RasterTiledLayer::minZoomlevel() const
{
    return 1; 
}

int RasterTiledLayer::maxZoomlevel() const 
{ 
    return 20; 
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
    if (!mDataset) return;

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
    QRectF image_rect = getMap()->getProjection()->geoToProj(mGeoExtent);

    tl::GeoTools *ptrGeoTools = tl::GeoTools::getInstance();
    OGRCoordinateTransformation *transform_to_raster = nullptr;
    OGRCoordinateTransformation *transform_to_3857 = nullptr;
    OGRSpatialReference targetRef;

    // Comprobar si hay cambio de CRS
    std::string epsg_source;
    {
        // Obtener CRS del shapefile
        const OGRSpatialReference *sourceRef = mDataset->GetSpatialRef();
        if (sourceRef) { // Si no tiene CRS tendría que devolver un error

            const char *auth_name = sourceRef->GetAuthorityName(nullptr);
            const char *auth_code = sourceRef->GetAuthorityCode(nullptr);

            if (auth_name && std::string(auth_name) == "EPSG" && auth_code) {

                int src_epsg = std::stoi(auth_code);
                int dst_epsg = 3857;
                epsg_source = std::string(auth_name).append(":").append(auth_code);

                if (src_epsg != dst_epsg) {

                    targetRef.importFromEPSG(4326);
                    
                    transform_to_raster = OGRCreateCoordinateTransformation(sourceRef, &targetRef);
                    //transform_to_3857 = OGRCreateCoordinateTransformation(&targetRef, sourceRef);

                }
            }
        }
    }

    int tileSize = 256; // Tamaño del tile en píxeles
    int tile_size_x_dst = tileSize;
    int tile_size_y_dst = tileSize;



    if (transform_to_raster) { // No estoy usando transform_to_raster...

        // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
        // la Y sale con el signo cambiado
        // Puntos de los extremos del tile en coordenadas proyectadas
        std::vector<tl::Point<double>> points(4);
        points[0] = tl::Point<double>(tile_proj_rect.left(), -tile_proj_rect.top());
        points[1] = tl::Point<double>(tile_proj_rect.right(), -tile_proj_rect.top());
        points[2] = tl::Point<double>(tile_proj_rect.right(), -tile_proj_rect.bottom());
        points[3] = tl::Point<double>(tile_proj_rect.left(), -tile_proj_rect.bottom());

        //TL_ASSERT(transform_to_3857->Transform(1, &x1, &y1) && transform_to_3857->Transform(1, &x2, &y2), "");
        // Hay que hacerlo con las 4 esquinas para tener realmente el bbox en el sistema origen
        points[0] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", epsg_source, points[0]);
        points[1] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", epsg_source, points[1]);
        points[2] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", epsg_source, points[2]);
        points[3] = ptrGeoTools->ptrCRSsTools()->crsOperation("EPSG:3857", epsg_source, points[3]);
        
        // Aqui hay que calcular la distancia entre los puntos para que este bien la escala
        double scale_src = 1.0;
        {
            auto p1 = mGeoreference.inverse().transform(static_cast<tl::Point2d>(points[0]));
            auto p2 = mGeoreference.inverse().transform(static_cast<tl::Point2d>(points[1]));
            auto p3 = mGeoreference.inverse().transform(static_cast<tl::Point2d>(points[3]));

            auto d1 = tl::distance(p1, p2);
            auto d2 = tl::distance(p1, p3);
            scale_src = std::min(tile_size_x_dst / static_cast<double>(d1), tile_size_y_dst / static_cast<double>(d2));
        }

        if (mScaleMin > scale_src) scale_src = mScaleMin;

        tl::WindowD source_window(points);
        tl::expandWindow(source_window, 2.0 / scale_src);
        //tl::BoundingBox<tl::Point3<double>> bbox_src(points);

        // Hay que expandir la ventana para evitar problemas en los bordes

        auto p1 = mGeoreference.inverse().transform(static_cast<tl::Point2d>(source_window.pt1));
        auto p2 = mGeoreference.inverse().transform(static_cast<tl::Point2d>(source_window.pt2));


        tl::Rect<int> rect_src(p1, p2);
        rect_src.normalized();

        //int xOffset = std::floor(std::min(p1.x, p2.x));
        //int yOffset = std::floor(std::min(p1.y, p2.y));

        double xSize = rect_src.width; //std::ceil(std::abs(p2.x - p1.x));
        double ySize = rect_src.height; //std::ceil(std::abs(p2.y - p1.y));

        //double sx = tile_size_x / static_cast<double>(xSize);
        //double sy = tile_size_y / static_cast<double>(ySize);
        //double scale_src = std::min(tile_size_x_dst / static_cast<double>(xSize), tile_size_y_dst / static_cast<double>(ySize));
        //int tile_size_x_src = tl::numberCast<int>(xSize * scale_src);
        //int tile_size_y_src = tl::numberCast<int>(ySize * scale_src);
        //int tile_offset_x_src = source_window.pt1.x;
        //int tile_offset_y_src = source_window.pt2.y;

        tl::Rect<int> rect_full_image(0, 0, reader->cols(), reader->rows());
        //tl::Rect<int> rect_src(xOffset, yOffset, xSize, ySize);
        tl::Rect<int> rect_to_read = tl::intersect(rect_full_image, rect_src);

        if (rect_to_read.width == 0 || rect_to_read.height == 0) return;

        //tl::Point<int> offset = rect_to_read.topLeft() - rect_src.topLeft();

        // Lo ideal sería tener un método que lea la imagen y haga la reproyección directamente
        cv::Mat mat1 = reader->read(scale_src, scale_src, rect_to_read);
        //tl::Size<int> size(tile_size_x_dst * scale_src, tile_size_x_dst * scale_src);
        //cv::Mat mat1 = reader->read(rect_to_read, size);
        
        /// Ventana leida en coordenadas fuente
        tl::Rect<double> rect_source(mGeoreference.transform(static_cast<tl::Point2d>(rect_to_read.topLeft())),
                                     mGeoreference.transform(static_cast<tl::Point2d>(rect_to_read.bottomRight())));
        rect_source.normalized();

        // 1. Crear dataset MEM temporal de origen
        GDALDriver *memDriver = GetGDALDriverManager()->GetDriverByName("MEM");
        if (memDriver == nullptr) {
            std::cerr << "Error: El driver 'MEM' no está disponible." << std::endl;
            return;
        }

        int nBands = mDataset->GetRasterCount();
        if (nBands == 0) {
            std::cerr << "Error: El dataset no contiene bandas raster." << std::endl;
            return;
        }

        GDALRasterBand *poSrcBand = mDataset->GetRasterBand(1);
        GDALDataType eDataType = GetGDALDataTypeFromBand(poSrcBand);
        if (eDataType == GDT_Unknown) {
            std::cerr << "Error: Tipo de dato de banda no soportado." << std::endl;
            return;
        }
        size_t nPixelSize = GDALGetDataTypeSize(eDataType) / 8; // Tamaño del pixel en bytes
        GDALDataset *srcDS = memDriver->Create("", mat1.cols, mat1.rows, nBands, eDataType, nullptr);
        if (srcDS == nullptr) {
            std::cerr << "Error: No se pudo crear el dataset en memoria para la región." << std::endl;
            return;
        }

        // 3. Calcular las coordenadas geográficas de la esquina superior izquierda de la región
        std::array<double, 6> adfGeoTransformRegion{};
        adfGeoTransformRegion[0] = rect_source.topLeft().x;
        adfGeoTransformRegion[1] = rect_source.width / static_cast<double>(mat1.cols);
        adfGeoTransformRegion[2] = 0.;
        adfGeoTransformRegion[3] = rect_source.bottomRight().y;
        adfGeoTransformRegion[4] = 0.;
        adfGeoTransformRegion[5] = -rect_source.height / static_cast<double>(mat1.rows);

        srcDS->SetGeoTransform(adfGeoTransformRegion.data());
        srcDS->SetProjection(mDataset->GetProjectionRef());

        std::vector<int> band_order{3, 2, 1};
        srcDS->RasterIO(GF_Write, 0, 0, mat1.cols, mat1.rows, 
                        mat1.ptr(), mat1.cols, mat1.rows,
                        GDT_Byte, 3, band_order.data(), 3, 3 * mat1.cols, 1);

        mat1.release();

        GDALDataset *warpedDS = static_cast<GDALDataset *>(GDALAutoCreateWarpedVRT(srcDS,
                                                                                   mDataset->GetProjectionRef(),  // fuente WKT
                                                                                   "EPSG:3857",                   // destino 
                                                                                    GRA_Bilinear, 0.0, nullptr));

        TL_ASSERT(warpedDS != nullptr, "Failed to reproject: {}", CPLGetLastErrorMsg());


        // Ahora creamos un dataset final en memoria para escribir los datos reproyectados
        // Necesitamos obtener las dimensiones finales del VRT
        int nDstWidth = warpedDS->GetRasterXSize();
        int nDstHeight = warpedDS->GetRasterYSize();

        GDALDataset *poDstMemDS = memDriver->Create("", nDstWidth, nDstHeight, nBands, eDataType, nullptr);
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
        if (warpedDS->GetGeoTransform(geoTransform) == CE_None) { // Las coordenadas Y están cambiadas de signo
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
        psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2(srcDS, poDstMemDS, nullptr);
        psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
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
        GDALDestroyGenImgProjTransformer(psWarpOptions->pTransformerArg);
        //CPLFree(pszTargetWKT); // Liberar el WKT generado

        // 9. Leer los datos del dataset reproyectado en memoria
        TransformedRegion result;
        result.nWidth = nDstWidth;
        result.nHeight = nDstHeight;
        result.nBands = nBands;
        result.eDataType = eDataType;

        double geo_transform_mem[6];
        poDstMemDS->GetGeoTransform(geo_transform_mem);

        //tile_proj_rect

        for (int i = 0; i < 6; ++i) {
            result.adfGeoTransform[i] = geo_transform_mem[i];
        }

        tl::Affine<double, 2> affine2;
        affine2(0, 0) = geo_transform_mem[1];
        affine2(0, 1) = geo_transform_mem[2];
        affine2(0, 2) = geo_transform_mem[0];
        affine2(1, 0) = geo_transform_mem[4];
        affine2(1, 1) = geo_transform_mem[5];
        affine2(1, 2) = geo_transform_mem[3];

        result.pszProjectionWKT = CPLStrdup(poDstMemDS->GetProjectionRef()); // Copiar el WKT

        size_t nTotalPixels = (size_t)nDstWidth * nDstHeight * nBands;
        result.data.resize(nTotalPixels * nPixelSize); // Ajustar el tamaño del vector

        CPLErr err = poDstMemDS->RasterIO(GF_Read,
                                          0, 0, nDstWidth, nDstHeight,
                                          result.data.data(), nDstWidth, nDstHeight,
                                          eDataType, nBands, nullptr,
                                          nPixelSize * nBands, nDstWidth * nPixelSize * nBands, 1);

        if (err != CE_None) {
            std::cerr << "Error: Falló la lectura del dataset de destino." << std::endl;
            //GDALClose(poDstMemDS);
            //GDALClose(warpedDS);
            //GDALClose(srcDS);
            //GDALClose(poSrcDS);
            return;
        }

        OCTDestroyCoordinateTransformation(transform_to_3857);

        p1 = affine2.inverse().transform(tl::Point2d(tile_proj_rect.left(), -tile_proj_rect.top()));
        p2 = affine2.inverse().transform(tl::Point2d(tile_proj_rect.right(), -tile_proj_rect.bottom()));

        tl::Rect<int> rect_dest(p1, p2);
        rect_dest.normalized();

        int xOffset = rect_dest.topLeft().x; //std::floor(std::min(p1.x, p2.x));
        int yOffset = rect_dest.topLeft().y; //std::floor(std::min(p1.y, p2.y));
        xSize = rect_dest.width; //std::ceil(std::abs(p2.x - p1.x));
        ySize = rect_dest.height;//std::ceil(std::abs(p2.y - p1.y));

        double sx = tile_size_x_dst / static_cast<double>(xSize);
        double sy = tile_size_y_dst / static_cast<double>(ySize);

        int tile_offset_x = 0;
        int tile_offset_y = 0;
        auto rows = result.nHeight;
        auto cols = result.nWidth;
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

        QImage tileImage(tileSize, tileSize, QImage::Format_RGBA8888);
        tileImage.fill(Qt::transparent);

        for (int y = 0; y < std::min(rows - yOffset, tileSize - tile_offset_y); ++y) {
            for (int x = 0; x < std::min(cols - xOffset, tileSize - tile_offset_x); ++x) {
                int i = (y + yOffset) * cols + x + xOffset;
                uint8_t r = result.data[i * 3 + 0];
                uint8_t g = result.data[i * 3 + 1];
                uint8_t b = result.data[i * 3 + 2];
                int alpha = 255;
                if (r == 0 && g == 0 && b == 0)
                    alpha = 0;
                tileImage.setPixelColor(tile_offset_x + x, tile_offset_y + y, QColor(r, g, b, alpha));
            }
        }
        
        cv::Mat mat2 = graphos::qImageToCvMat(tileImage);

        if (tileImage.isNull()) {
            qWarning() << "No se pudo cargar la imagen del tile:" << tilePos.pos();
            return;
        }

        auto *imageItem = new QGVImage();
        imageItem->setGeometry(tilePos.toGeoRect()/*tile_proj_rect*/);
        imageItem->loadImage(tileImage);
        //imageItem->setCeilingOnScale(false);
        imageItem->setSelectable(false);
        onTile(tilePos, imageItem);

    } else {

        // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
        // la Y sale con el signo cambiado
        auto p1 = mGeoreference.inverse().transform({tile_proj_rect.left(), -tile_proj_rect.top()});
        auto p2 = mGeoreference.inverse().transform({tile_proj_rect.right(), -tile_proj_rect.bottom()});

        int xOffset = std::floor(std::min(p1.x, p2.x));
        int yOffset = std::floor(std::min(p1.y, p2.y));
        int xSize = std::ceil(std::abs(p2.x - p1.x));
        int ySize = std::ceil(std::abs(p2.y - p1.y));


        double sx = tile_size_x_dst / static_cast<double>(xSize);
        double sy = tile_size_y_dst / static_cast<double>(ySize);

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

        uint8_t *buffer = new uint8_t[tile_size_x_dst * tile_size_y_dst * 3];

        CPLErr err = mDataset->RasterIO(GF_Read,
                                        xOffset, yOffset, xSize, ySize,
                                        buffer, tile_size_x_dst, tile_size_y_dst,
                                        GDT_Byte, 3, nullptr,
                                        3, tile_size_x_dst * 3, 1);
        if (err == CE_None) {
            for (int y = 0; y < tile_size_y_dst; ++y) {
                for (int x = 0; x < tile_size_x_dst; ++x) {
                    int i = y * tile_size_x_dst + x;
                    uint8_t r = buffer[i * 3 + 0];
                    uint8_t g = buffer[i * 3 + 1];
                    uint8_t b = buffer[i * 3 + 2];
                    int alpha = 255;
                    if (r == 0 && g == 0 && b == 0)
                        alpha = 0;
                    tileImage.setPixelColor(tile_offset_x + x, tile_offset_y + y, QColor(r, g, b, alpha));
                }
            }
        } else { return; }

        if (tileImage.isNull()) {
            qWarning() << "No se pudo cargar la imagen del tile:" << tilePos.pos();
            delete[] buffer;
            return;
        }

        //cv::Mat mat2 = graphos::qImageToCvMat(tileImage);

        auto tile = new QGVImage();
        tile->setGeometry(tilePos.toGeoRect()/*tile_proj_rect*/);
        tile->loadImage(tileImage);
        //tile->setCeilingOnScale(false);
        tile->setSelectable(false);
        onTile(tilePos, tile);

        delete[] buffer;
    }

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