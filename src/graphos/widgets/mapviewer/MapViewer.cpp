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

#include "graphos/widgets/mapviewer/MapViewer.h"

#include "graphos/widgets/mapviewer/MapPolygon.h"
#include "graphos/widgets/mapviewer/RasterTiledLayer.h"
#include "graphos/widgets/mapviewer/MapContextMenu.h"

/* QGeoView */
#include <QGeoView/QGVMap.h>
#include <QGeoView/QGVLayer.h>
#include <QGeoView/Raster/QGVImage.h>
#include <QGeoView/QGVLayerOSM.h>
#include <QGeoView/QGVLayerGoogle.h>
#include <QGeoView/QGVLayerBing.h>
#include <QGeoView/QGVWidgetScale.h>

/* Qt */
#include <QVBoxLayout>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>

/* GDAL */
#include <gdal.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
//#include <gdalwarper.h>

//#include <QBrush>
//#include <QPen>
//#include <QTimer>
//#include <QPainter>
#include <QSslSocket>
#include <QDebug>



namespace graphos
{

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

MapViewer::MapViewer(QWidget *parent)
  : QWidget(parent),
    mMap(new QGVMap(this)),
    mContextMenu(new MapContextMenu(this))
{
    init();
    initSignalsAndSlots();
}

MapViewer::~MapViewer()
{
}

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
}

void MapViewer::loadGeoTiff(const QString &tifPath, bool dem)
{
    try {

        for (int i = 0; i < mMap->countItems(); ++i) {
            QGVItem *item = mMap->getItem(i);
            if (dynamic_cast<RasterTiledLayer *>(item)) {
                mMap->removeItem(item);
                delete item;
            }
        }

        auto layer = new RasterTiledLayer(tifPath, dem);
        layer->setVisibleZoomLayersAboveCurrent(0);
        layer->setVisibleZoomLayersBelowCurrent(0);
        mMap->addItem(layer);

        auto max_geo_extend = layer->maxGeoExtent();
        max_geo_extend.topLeft();
        mVectorExtent |= QRectF(QPointF(max_geo_extend.topLeft().latitude(), max_geo_extend.topLeft().longitude()),
            QPointF(max_geo_extend.bottomRight().latitude(), max_geo_extend.bottomRight().longitude()));

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MapViewer::zoomExtend()
{
    auto &action = QGVCameraActions(mMap).scaleTo(maxGeoExtent());
    mMap->cameraTo(action);
    //mMap->flyTo(action);
}

void MapViewer::zoomIn()
{
    auto scale1 = QGVCameraActions(mMap).scale();
    auto &action = QGVCameraActions(mMap).scaleBy(2.0);
    mMap->cameraTo(action);
    auto scale2 = QGVCameraActions(mMap).scale();
    scale2 *= 1;
    //mMap->flyTo(action);
}

void MapViewer::zoomOut()
{
    auto &action = QGVCameraActions(mMap).scaleBy(0.5);
    mMap->cameraTo(action);
    //mMap->flyTo(action);
}

void MapViewer::changeBaseMap(BaseMap baseMap)
{
    //mBaseLayerOSM->hide();
    mBaseLayerGoogleSatellite->hide();
    mBaseLayerGoogleHybrid->hide();
    mBaseLayerGoogleSchema->hide();
    mBaseLayerBingSatellite->hide();
    mBaseLayerBingHybrid->hide();
    mBaseLayerBingSchema->hide();

    switch (baseMap) {
    //case graphos::MapViewer::BaseMap::osm:
    //    mBaseLayerOSM->show();
    //    break;
    case graphos::MapViewer::BaseMap::google_schema:
        mBaseLayerGoogleSchema->show();
        break;
    case graphos::MapViewer::BaseMap::google_satellite:
        mBaseLayerGoogleSatellite->show();
        break;
    case graphos::MapViewer::BaseMap::google_hybrid:
        mBaseLayerGoogleHybrid->show();
        break;
    case graphos::MapViewer::BaseMap::bing_schema:
        mBaseLayerBingSchema->show();
        break;
    case graphos::MapViewer::BaseMap::bing_satellite:
        mBaseLayerBingSatellite->show();
        break;
    case graphos::MapViewer::BaseMap::bing_hybrid:
        mBaseLayerBingHybrid->show();
        break;
    default:
        break;
    }
}

void MapViewer::showContextMenu(const QPoint &position)
{
    QPoint global_pos = mapToGlobal(position);
    mContextMenu->exec(global_pos);
}

void MapViewer::init()
{
    qDebug() << "SSL support:" << QSslSocket::supportsSsl();
    qDebug() << "Build:" << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "Runtime:" << QSslSocket::sslLibraryVersionString();

    QDir("cacheDir").removeRecursively();
    auto cache = new QNetworkDiskCache(this);
    cache->setCacheDirectory("cacheDir");
    auto manager = new QNetworkAccessManager(this);
    manager->setCache(cache);
    QGV::setNetworkManager(manager);

    //mBaseLayerOSM = new QGVLayerOSM();
    //mBaseLayerOSM->show();
    //mMap->addItem(mBaseLayerOSM);
    mBaseLayerGoogleSchema = new QGVLayerGoogle(QGV::TilesType::Schema);
    mBaseLayerGoogleSchema->show();
    mMap->addItem(mBaseLayerGoogleSchema);
    mBaseLayerGoogleSatellite = new QGVLayerGoogle(QGV::TilesType::Satellite);
    mBaseLayerGoogleSatellite->hide();
    mMap->addItem(mBaseLayerGoogleSatellite);
    mBaseLayerGoogleHybrid = new QGVLayerGoogle(QGV::TilesType::Hybrid);
    mBaseLayerGoogleHybrid->hide();
    mMap->addItem(mBaseLayerGoogleHybrid);
    mBaseLayerBingSchema = new QGVLayerBing(QGV::TilesType::Schema);
    mBaseLayerBingSchema->hide();
    mMap->addItem(mBaseLayerBingSchema);
    mBaseLayerBingSatellite = new QGVLayerBing(QGV::TilesType::Satellite);
    mBaseLayerBingSatellite->hide();
    mMap->addItem(mBaseLayerBingSatellite);
    mBaseLayerBingHybrid = new QGVLayerBing(QGV::TilesType::Hybrid);
    mBaseLayerBingHybrid->hide();
    mMap->addItem(mBaseLayerBingHybrid);

    mMap->addWidget(new QGVWidgetScale());

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mMap);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

void MapViewer::initSignalsAndSlots()
{
    connect(this, &MapViewer::customContextMenuRequested, this, &MapViewer::showContextMenu);

    //connect(mContextMenu, &MapContextMenu::baseMapOSM, this, 
    //    [this]() {
    //        changeBaseMap(BaseMap::osm);
    //    });
    
    connect(mContextMenu, &MapContextMenu::baseMapGoogleSchema, this,
        [this]() {
            changeBaseMap(BaseMap::google_schema);
        });

    connect(mContextMenu, &MapContextMenu::baseMapGoogleHybrid, this, 
        [this]() {
            changeBaseMap(BaseMap::google_hybrid);
        });
    
    connect(mContextMenu, &MapContextMenu::baseMapGoogleSatellite, this, 
        [this]() {
            changeBaseMap(BaseMap::google_satellite);
        });

    connect(mContextMenu, &MapContextMenu::baseMapBingSchema, this,
        [this]() {
            changeBaseMap(BaseMap::bing_schema);
        });

    connect(mContextMenu, &MapContextMenu::baseMapBingHybrid, this,
        [this]() {
            changeBaseMap(BaseMap::bing_hybrid);
        });

    connect(mContextMenu, &MapContextMenu::baseMapBingSatellite, this,
        [this]() {
            changeBaseMap(BaseMap::bing_satellite);
        });

    connect(mContextMenu, &MapContextMenu::zoomIn, this, &MapViewer::zoomIn);
    connect(mContextMenu, &MapContextMenu::zoomOut, this, &MapViewer::zoomOut);
    connect(mContextMenu, &MapContextMenu::zoomExtend, this, &MapViewer::zoomExtend);
}

QGV::GeoRect MapViewer::maxGeoExtent() const
{
    return QGV::GeoRect(QGV::GeoPos(mVectorExtent.topLeft().x(), mVectorExtent.topLeft().y()),
                        QGV::GeoPos(mVectorExtent.bottomRight().x(), mVectorExtent.bottomRight().y()));
}


}