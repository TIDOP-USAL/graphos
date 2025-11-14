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

#include <tidop/math/geometry/affine.h>

//class GDALDataset;

namespace tl
{
class ImageReader;
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

    QGV::GeoRect maxGeoExtent() const;

protected:

    void onProjection(QGVMap *geoMap) override;
    int minZoomlevel() const override;
    int maxZoomlevel() const override;
    void request(const QGV::GeoTilePos &tilePos) override;
    void cancel(const QGV::GeoTilePos &tilePos) override;

private:

    QRect mTileGridBounds;
    QGV::GeoRect mGeoExtent;
    std::unique_ptr<tl::ImageReader> reader;
    tl::Affine<double, 2> mGeoreference;
    double mGSD;
    bool mCrsTransform;
    std::string mEpsgSource;
    std::pair<double, double> mMinMax;
    bool mDEM;

};


}

#endif // GRAPHOS_RASTER_TILED_LAYER_H