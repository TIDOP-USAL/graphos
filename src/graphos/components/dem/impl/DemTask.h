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

#pragma once

#include <QObject>

#include <tidop/core/task/Task.h>
#include <tidop/core/task/Progress.h>

#include "graphos/core/dem/DemReport.h"
#include "graphos/core/dem/DemProperties.h"

namespace graphos
{

class DemTask
  : public QObject,
    public tl::Task
{

    Q_OBJECT

public:

    DemTask(tl::Path pointCloud,
            tl::Path demPath,
            std::string enuCrs,
            const std::shared_ptr<DemProperties> &properties);

    ~DemTask() override = default;

    TL_DISABLE_COPY(DemTask)
    TL_DISABLE_MOVE(DemTask)

    /*!
     * \brief Get the DEM report after task execution.
     *
     * \return A 'DemReport' containing information about the DEM task.
     */
    auto report() const -> DemReport;

    auto dsmPath() const -> tl::Path;
    auto dtmPath() const -> tl::Path;
    auto dsmEnuPath() const -> tl::Path;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    tl::Path mPointCloud;
    tl::Path mDemPath;
    tl::Path mDsmPath;
    tl::Path mDtmPath;
    tl::Path mDsmEnuPath;
    std::string mEnuCrs;
    std::shared_ptr<DemProperties> mProperties;
    DemReport mDemReport;
};

} // namespace graphos
