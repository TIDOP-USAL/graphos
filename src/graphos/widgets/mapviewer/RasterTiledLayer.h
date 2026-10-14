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

#ifndef GRAPHOS_RASTER_TILED_LAYER_H
#define GRAPHOS_RASTER_TILED_LAYER_H


#include <QGeoView/QGVLayerTiles.h>
#include <QImage>
#include <QMap>
#include <QThreadPool>

#include <tidop/geometry/transform/Affine.h>

#include <opencv2/core.hpp>

#include <atomic>
#include <memory>
#include <mutex>

namespace tl
{
class RasterReader;
}

namespace graphos
{


class RasterTiledLayer 
  : public QGVLayerTiles
{
    Q_OBJECT

public:

    RasterTiledLayer(const QString &tifPath, bool dem = false);
    ~RasterTiledLayer();

    auto maxGeoExtent() const -> QGV::GeoRect;

protected:

    void onProjection(QGVMap *geoMap) override;
    auto minZoomlevel() const -> int override;
    auto maxZoomlevel() const -> int override;
    void request(const QGV::GeoTilePos &tilePos) override;
    void buildDEM(cv::Mat &mat_source);
    void cancel(const QGV::GeoTilePos &tilePos) override;

private:

    struct TileRequest
    {
        quint64 id;
        std::shared_ptr<std::atomic_bool> cancelled;
    };

    auto renderTile(const QGV::GeoTilePos &tilePos, const QRectF &tileProjRect) -> QImage;
    void finishTile(const QGV::GeoTilePos &tilePos, quint64 requestId, QImage image);

    QRect mTileGridBounds;
    QGV::GeoRect mGeoExtent;
    QRectF mProjectedExtent;
    std::unique_ptr<tl::RasterReader> reader;
    QThreadPool mThreadPool;
    QMap<QGV::GeoTilePos, TileRequest> mRequests;
    std::mutex mReaderMutex;
    tl::Affine<double, 2> mAffineImageToWorld;
    quint64 mNextRequestId{0};
    int mRasterRows{0};
    int mRasterCols{0};
    double mNoDataValue{0.};
    double mCellSize{1.};
    bool mHasNoDataValue{false};
    double mGSD{0.};
    bool mCrsTransform{false};
    std::string mEpsgSource;
    std::pair<double, double> mMinMax{0., 0.};
    bool mDEM{false};

};


}

#endif // GRAPHOS_RASTER_TILED_LAYER_H