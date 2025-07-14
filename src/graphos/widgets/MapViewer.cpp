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

#include <tidop/math/geometry/affine.h>
#include <tidop/img/imgreader.h>
#include <tidop/geometry/size.h>
#include <tidop/geometry/entities/point.h>

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

        // Obtener CRS del shapefile
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

    // QGeoView tiene un error en la transformación entre geograficas y EPSG:3857 y 
    // la Y sale con el signo cambiado
    auto p1 =affine.inverse().transform({tile_proj_rect.left(), -tile_proj_rect.top()}); 
    auto p2 = affine.inverse().transform({tile_proj_rect.right(), -tile_proj_rect.bottom()});

    int xOffset = std::floor(std::min(p1.x, p2.x));
    int yOffset = std::floor(std::min(p1.y, p2.y));
    int xSize = std::ceil(std::abs(p2.x - p1.x));
    int ySize = std::ceil(std::abs(p2.y - p1.y));

    int tileSize = 256; // Tamaño del tile en píxeles
    int tile_size_x = tileSize;
    int tile_size_y = tileSize;
    double sx = tile_size_x / static_cast<double>(xSize);
    double sy = tile_size_y / static_cast<double>(ySize);

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

    //tl::Affine<int, 2> affine_out;
    //reader->read(window, scale_x, scale_y, &affine_out);
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