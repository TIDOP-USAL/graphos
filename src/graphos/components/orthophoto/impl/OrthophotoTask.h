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

#ifndef GRAPHOS_ORTHOPHOTO_PROCESS_H
#define GRAPHOS_ORTHOPHOTO_PROCESS_H

#include <vector>
#include <map>
#include <unordered_map>

#include <QObject>

#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>
#include <tidop/geometry/entities/window.h>
#include <tidop/img/img.h>

#include "graphos/core/reports/orthophoto.h"

namespace tl 
{
class Progress;
}

namespace graphos
{

class Image;
class Camera;

class OrthophotoTask
  : public QObject,
    public tl::TaskBase
{

public:

    OrthophotoTask(const std::unordered_map<size_t, Image> &images,
                   const std::map<int, Camera> &cameras,
                   const tl::Path &orthoPath,
                   const tl::Path &mdt,
                   const std::string &enuCrs,
                   const std::string &epsg,
                   const std::string &interpolation = "BILINEAR",
                   double resolution = -1.,
                   bool cuda = false);

    ~OrthophotoTask() override;

    auto report() const -> OrthophotoReport;

private:

    //std::vector<tl::WindowD> findGrid(const tl::Path &mdt, double gsd);
    std::vector<std::vector<tl::WindowD>> findGrid(int gridSize) const;



    //void orthoimageExposureCompensator(const tl::Path &graph_orthos, tl::Progress *progressBar);
    auto searchTiles(const tl::Path &graph_orthos,
                     const std::vector<std::vector<tl::WindowD>> &grid,
                     int maxImages = 0) -> std::vector<std::vector<std::map<double, std::string>>>;
    void generateTiles(const std::vector<std::vector<tl::WindowD>> &grid, 
                       std::vector<std::vector<std::map<double, std::string>>> &orthos,
                       tl::Progress *progressBar);
    //void tilesExposureCompensator(const std::vector<std::vector<tl::WindowD>> &grid);
    void writeOrthomosaic(const std::vector<std::vector<tl::WindowD>> &grid);
    //void blendTileBlock(int r, int c, const std::vector<std::vector<tl::WindowD>> &grid);

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    std::unordered_map<size_t, Image> mPhotos;
    std::map<int, Camera> mCameras;
    tl::Path mOrthoPath;
    tl::Path mMdt;
    std::string mEnuCrs;
    std::string mEpsg;
    std::string mInterpolation;
    OrthophotoReport mOrthophotoReport;
    double mGSD;
    bool bCuda;
    // Por ahora se calcula internamente pero se podría establecer la región de la ortofoto externamente
    tl::WindowD mWindowAll;
    tl::DataType mDataType;
    int mChannels;
    std::string mMethod;
};

} // namespace graphos

#endif // GRAPHOS_ORTHOPHOTO_PROCESS_H
