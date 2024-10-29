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

#ifndef GRAPHOS_EXPORT_POINT_CLOUD_TASK_H
#define GRAPHOS_EXPORT_POINT_CLOUD_TASK_H

#include <tidop/core/task.h>
#include <tidop/core/progress.h>
#include <tidop/geometry/entities/point.h>

#include <QObject>

namespace graphos
{

class ExportPointCloudTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    ExportPointCloudTask(tl::Path pointCloud,
                         tl::Point3<double> offset,
                         tl::Path exportPointCloud,
                         std::string crs);

    ~ExportPointCloudTask() override = default;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mPointCloud;
    tl::Point3<double> mOffset;
    tl::Path mExportPointCloud;
    std::string mCrs;
};


} // namespace graphos

#endif // GRAPHOS_EXPORT_POINT_CLOUD_TASK_H
