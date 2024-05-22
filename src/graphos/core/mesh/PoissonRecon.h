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

#ifndef GRAPHOS_POISSONRECON_ALGORITHM_H
#define GRAPHOS_POISSONRECON_ALGORITHM_H

#include "graphos/graphos_global.h"

#include "graphos/core/utils.h"

#include <tidop/core/task.h>
#include <tidop/core/path.h>

namespace tl
{
  class Process;
}

namespace graphos
{

    
struct MeshReport
{
    double time = 0.0;

    bool isEmpty() const
    {
        return time == 0.;
    }
};


/*!
 * \brief Properties for Poisson Surface Reconstruction.
 *
 * This class provides properties and settings for Poisson Surface Reconstruction algorithm.
 *
 * For more information on Poisson Surface Reconstruction, refer to:
 * - Paper: https://hhoppe.com/poissonrecon.pdf
 * - GitHub Repository: https://github.com/mkazhdan/PoissonRecon
 */
class PoissonReconProperties
{
public:

    enum class BoundaryType
    {
        free,
        dirichlet,
        neumann
    };

public:

    /*!
     * \brief Default constructor for PoissonReconProperties.
     * Constructs a PoissonReconProperties object with default values.
     */
    PoissonReconProperties();
    virtual ~PoissonReconProperties();

    /*!
     * \brief Gets the depth parameter.
     * This is the maximum depth of the tree that will be used for surface
     * reconstruction. The default value for this parameter is 8.
     * \return The depth parameter.
     */
    virtual auto depth() const -> int;

    /*!
     * \brief Gets the solve depth parameter.
     * \return The solve depth parameter.
     */
    //virtual auto solveDepth() const -> int;

    /*!
     * \brief Gets the boundary type parameter.
     * This parameter specifies the boundary type for the finite elements. Valid values are:
     * - free: Free boundary constraints
     * - dirichlet: Dirichlet boundary constraints
     * - neumann: Neumann boundary constraints
     * The default value for this parameter is neumann.
     * \return The boundary type parameter.
     * \see BoundaryType
     */
    virtual auto boundaryType() const -> BoundaryType;

    virtual auto boundaryTypeAsText() const -> QString;

    /*!
     * \brief Sets the depth parameter.
     * \param[in] depth Depth parameter value.
     */
    virtual void setDepth(int depth);

    /*!
     * \brief Sets the solve depth parameter.
     * \param[in] solveDepth The solve depth parameter to set.
     */
    //virtual void setSolveDepth(int solveDepth);

    /*!
     * \brief Sets the boundary type parameter.
     * \param[in] boundaryType The boundary type parameter to set.
     * \see BoundaryType
     */
    virtual void setBoundaryType(BoundaryType boundaryType);

    void clear();

private:

    int mDepth;
    //int mSolveDepth;
    BoundaryType mBoundaryType;

};




/*!
 * \brief Poisson Surface Reconstruction Task.
 *
 * This class represents a task for performing Poisson Surface Reconstruction. It inherits functionality from both tl::TaskBase
 * for task management and PoissonReconProperties for configuring Poisson Surface Reconstruction parameters.
 * The task takes an input path where the input data is stored and an output path where the reconstructed mesh will be saved.
 */
class PoissonReconTask final
  : public tl::TaskBase,
    public PoissonReconProperties
{

public:

    /*!
     * \brief Constructor for PoissonReconTask.
     * Constructs a PoissonReconTask object with the provided input and output paths.
     * \param[in] input The path to the input data.
     * \param[in] output The path where the reconstructed mesh will be saved.
     */
    PoissonReconTask(tl::Path input,
                     tl::Path output);

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

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mInput;
    tl::Path mOutput;
    MeshReport mReport;
};



} // namespace graphos


#endif // GRAPHOS_POISSONRECON_ALGORITHM_H
