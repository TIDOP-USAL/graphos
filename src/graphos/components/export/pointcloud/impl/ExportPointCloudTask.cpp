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

#include "graphos/core/ply.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/task/Progress.h"


namespace graphos
{



ExportPointCloudTask::ExportPointCloudTask(tl::Path pointCloud,
                                           tl::Point3<double> offset,
                                           tl::Path exportPointCloud,
                                           std::string crs)
  : tl::TaskBase(),
    mPointCloud(std::move(pointCloud)),
    mOffset(std::move(offset)),
    mExportPointCloud(std::move(exportPointCloud)),
    mCrs(std::move(crs))
{

}

void ExportPointCloudTask::execute(tl::Progress *progressBar)
{

    try {

        TL_ASSERT(mPointCloud.exists(), "Point cloud file not exist: '{}'", mPointCloud.toString().c_str());
        TL_ASSERT(mPointCloud.isFile(), "The path is not valid: '{}'", mPointCloud.toString().c_str());

        mExportPointCloud.parentPath().createDirectories();

        tl::Point3<double> ecef_center = mOffset;

        auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
        auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");

        tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
        auto lla = crs_transfom_geocentric_to_geographic.transform(ecef_center);
        auto rotation = tl::rotationEnuToEcef(lla.x, lla.y);
        std::shared_ptr<tl::EcefToEnu> ecef_to_enu = std::make_shared<tl::EcefToEnu>(ecef_center, rotation);

        auto epsg_utm = std::make_shared<tl::Crs>(mCrs);
        tl::CrsTransform crs_transfom(epsg_geocentric, epsg_utm);

        Ply ply_reader(mPointCloud, Ply::OpenMode::in);
        size_t size = ply_reader.size();
        ply_reader.read();

        if (progressBar) (*progressBar)(10);

        Ply ply;
        ply.open(mExportPointCloud, Ply::OpenMode::out);
        ply.setProperty("x", PlyProperty::ply_double);
        ply.setProperty("y", PlyProperty::ply_double);
        ply.setProperty("z", PlyProperty::ply_double);
        if (ply_reader.hasColors()) {
            ply.setProperty("red", PlyProperty::ply_int);
            ply.setProperty("green", PlyProperty::ply_int);
            ply.setProperty("blue", PlyProperty::ply_int);
        }
        if (ply_reader.hasNormals()) {
            ply.setProperty("nx", PlyProperty::ply_float);
            ply.setProperty("ny", PlyProperty::ply_float);
            ply.setProperty("nz", PlyProperty::ply_float);
        }

        for (size_t i = 0; i < size; i++) {

            auto point = ply_reader.point<double>(i);
            auto point_ecef = ecef_to_enu->inverse(point);
            auto point_utm = crs_transfom.transform(point_ecef);
            ply.addPoint<double>(point_utm);
            if (ply_reader.hasColors())
                ply.addColor(ply_reader.color(i));
            if (ply_reader.hasNormals())
                ply.addNormals<float>(ply_reader.normals<float>(i));

            if (progressBar) (*progressBar)();
        }

        ply.save(true);

        ply_reader.close();
        ply.close();

        tl::Message::success("Point cloud export finished in {:.2} minutes", this->time() / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("DTM tast error");
    }

}



} // graphos
