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

#include <QObject>

#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/window.h>

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

    OrthophotoTask(double resolution,
                   const std::vector<Image> &images,
                   const std::map<int, Camera> &cameras,
                   const tl::Path &orthoPath,
                   const tl::Path &mdt,
                   const std::string &enuCrs,
                   const std::string &epsg,
                   const std::string &interpolation = "BILINEAR",
                   bool cuda = false);

    ~OrthophotoTask() override;

    auto report() const -> OrthophotoReport;

    void setGSD(double gsd);
    void setPhotos(const std::vector<Image> &images);
    void setOrthoPath(const tl::Path &orthoPath);
    void setMdt(const tl::Path &mdt);
    void setCrs(const std::string &epsg);
    void setCuda(bool active);

private:

    std::vector<tl::WindowD> findGrid(const tl::Path &mdt, double gsd);
    std::vector<std::vector<tl::WindowD>> findGrid2(const tl::Path &mdt, double gsd, int gridSize);

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    double mGSD;
    std::vector<Image> mPhotos;
    std::map<int, Camera> mCameras;
    tl::Path mOrthoPath;
    tl::Path mMdt;
    std::string mEnuCrs;
    std::string mEpsg;
    std::string mInterpolation;
    OrthophotoReport mOrthophotoReport;
    bool bCuda;

};

} // namespace graphos

#endif // GRAPHOS_ORTHOPHOTO_PROCESS_H
