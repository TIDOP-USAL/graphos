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

#ifndef GRAPHOS_DEM_TASK_H
#define GRAPHOS_DEM_TASK_H

#include <QObject>

#include <tidop/core/task/Task.h>
#include <tidop/core/task/Progress.h>
#include <tidop/geometry/primitives/Point.h>

#include "graphos/core/reports/dem.h"

namespace graphos
{

class DemTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    DemTask(tl::Path pointCloud,
            std::string enuCrs,
            std::string crsOut,
            tl::Path demPath,
            double gsd,
            bool dsm,
            bool dtm);

    ~DemTask() override = default;

    /*!
     * \brief Get the DEM report after task execution.
     *
     * \return A 'DemReport' containing information about the DEM task.
     */
    auto report() const -> DemReport;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mPointCloud;
    std::string mEnuCrs;
    std::string mCrs;
    tl::Path mDemPath;
    double mGsd;
    bool mDsm;
    bool mDtm;
    DemReport mDemReport;
};


} // namespace graphos

#endif // GRAPHOS_DEM_TASK_H
