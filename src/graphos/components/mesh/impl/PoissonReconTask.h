/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>

#include "graphos/core/mesh/PoissonReconProperties.h"
#include "graphos/core/mesh/MeshReport.h"

namespace graphos
{

/*!
 * \brief Poisson Surface Reconstruction Task.
 *
 * This class represents a task for performing Poisson Surface Reconstruction. It inherits functionality from both tl::TaskBase
 * for task management and PoissonReconProperties for configuring Poisson Surface Reconstruction parameters.
 * The task takes an input path where the input data is stored and an output path where the reconstructed mesh will be saved.
 */
class PoissonReconTask final
  : public tl::Task
{

public:

    /*!
     * \brief Constructor for PoissonReconTask.
     * Constructs a PoissonReconTask object with the provided input and output paths.
     * \param[in] input The path to the input data.
     * \param[in] output The path where the reconstructed mesh will be saved.
     */
    PoissonReconTask(tl::Path input,
                     tl::Path output,
                     const std::shared_ptr<PoissonReconProperties> &properties);

    /*!
     * \brief Destructor for PoissonReconTask.
     */
    ~PoissonReconTask() override;

    /*!
     * \brief Generates a report on the reconstructed mesh.
     * \return The report on the reconstructed mesh.
     */
    auto report() const -> MeshReport;

private:

    void poissonRecon(const tl::Path &app_path) const;
    void surfaceTrimmer(const tl::Path &app_path) const;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

private:

    tl::Path mInput;
    tl::Path mOutput;
    std::shared_ptr<PoissonReconProperties> mProperties;
    MeshReport mReport;
};

} // namespace graphos
