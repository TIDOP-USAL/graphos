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

#ifndef GRAPHOS_EXPORT_POINT_CLOUD_MODEL_H
#define GRAPHOS_EXPORT_POINT_CLOUD_MODEL_H

#include "graphos/components/export/pointcloud/ExportPointCloudModel.h"

namespace graphos
{

class Project;

class ExportPointCloudModelImp
  : public ExportPointCloudModel
{

    Q_OBJECT

public:

    ExportPointCloudModelImp(Project *project,
                             QObject *parent = nullptr);
    ~ExportPointCloudModelImp() override;

// ExportPointCloudModel interface

public:

    auto graphosProjectsDirectory() const -> tl::Path override;

public slots:

    void exportPointCloud(const tl::Path &exportPath) override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_EXPORT_POINT_CLOUD_MODEL_H
