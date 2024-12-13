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

#include "ExportPointCloudTask.h"

#include <tidop/geospatial/crstransf.h>
#include <tidop/GeoTools/CRSsTools.h>
#include <tidop/GeoTools/GeoTools.h>

#include "graphos/core/ply.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/task/Progress.h"


namespace graphos
{



ExportPointCloudTask::ExportPointCloudTask(tl::Path pointCloud,
                                           tl::Path exportPointCloud,
                                           std::string crsEnu,
                                           std::string crsOut,
                                           bool bynary,
                                           bool colors,
                                           bool normals)
  : tl::TaskBase(),
    mPointCloud(std::move(pointCloud)),
    mExportPointCloud(std::move(exportPointCloud)),
    mCrsEnu(std::move(crsEnu)),
    mCrs(std::move(crsOut)),
    mBynary(bynary),
    mColors(colors),
    mNormals(normals)
{

}

void ExportPointCloudTask::execute(tl::Progress *progressBar)
{

    try {

        TL_ASSERT(mPointCloud.exists(), "Point cloud file not exist: '{}'", mPointCloud.toString().c_str());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString().c_str());

        mExportPointCloud.parentPath().createDirectories();

        Ply ply_reader(mPointCloud, Ply::OpenMode::in);
        size_t size = ply_reader.size();
        ply_reader.read();

        
        if (progressBar) (*progressBar)(static_cast<size_t>(size / 80.) * 10);

        Ply ply;
        ply.open(mExportPointCloud, Ply::OpenMode::out);
        ply.setProperty("x", PlyProperty::ply_double);
        ply.setProperty("y", PlyProperty::ply_double);
        ply.setProperty("z", PlyProperty::ply_double);
        if (ply_reader.hasColors() && mColors) {
            ply.setProperty("red", PlyProperty::ply_int);
            ply.setProperty("green", PlyProperty::ply_int);
            ply.setProperty("blue", PlyProperty::ply_int);
        }
        if (ply_reader.hasNormals() && mNormals) {
            ply.setProperty("nx", PlyProperty::ply_float);
            ply.setProperty("ny", PlyProperty::ply_float);
            ply.setProperty("nz", PlyProperty::ply_float);
        }

        tl::GeoTools *ptrGeoTools = tl::GeoTools::getInstance();

        for (size_t i = 0; i < size; i++) {

            auto point = ply_reader.point<double>(i);
            ptrGeoTools->ptrCRSsTools()->crsOperation(mCrsEnu, mCrs, point.x, point.y, point.z);
            ply.addPoint<double>(point);
            if (ply_reader.hasColors() && mColors)
                ply.addColor(ply_reader.color(i));
            if (ply_reader.hasNormals() && mNormals)
                ply.addNormals<float>(ply_reader.normals<float>(i));

            if (progressBar) (*progressBar)();
        }

        ply.save(mBynary);

        ply_reader.close();
        ply.close();

        tl::Message::success("Point cloud export finished in {:.2} minutes", this->time() / 60.);

        if (progressBar) (*progressBar)(static_cast<size_t>(static_cast<double>(size) / 80.) * 10);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Export point cloud task error");
    }

}



} // graphos
