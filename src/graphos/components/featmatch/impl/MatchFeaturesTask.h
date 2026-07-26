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

#include "graphos/graphos_global.h"

#include <stop_token>

#include <QObject>

#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>

#include "graphos/core/features/FeatureMatching.h"
#include "graphos/core/features/FeatureMatchingReport.h"

namespace graphos
{

class MatchFeaturesTask
  : public QObject,
    public tl::Task
{

    Q_OBJECT

public:

    MatchFeaturesTask(tl::Path database,
                        bool cuda,
                        const std::shared_ptr<FeatureMatching> &featureMatching);
    ~MatchFeaturesTask() override;

public:

    auto report() const -> FeatureMatchingReport;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    tl::Path mDatabase;
    bool bUseCuda;
    std::shared_ptr<FeatureMatching> mFeatureMatching;
    FeatureMatchingReport mReport;

};


} // namespace graphos
