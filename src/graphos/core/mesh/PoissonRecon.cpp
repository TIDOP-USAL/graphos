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

#include "PoissonRecon.h"

#include "graphos/core/AppStatus.h"

#include <tidop/core/exception.h>
#include <tidop/core/app.h>
#include <tidop/core/path.h>
#include <tidop/core/progress.h>
#include <tidop/core/chrono.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>


using namespace CGAL;


namespace graphos
{


/// valores por defecto

constexpr auto default_poisson_recon_depth = 14;
constexpr auto default_poisson_recon_solve_depth = 13;
constexpr auto default_poisson_recon_boundary_type = PoissonReconProperties::BoundaryType::neumann;


/* PoissonReconProperties */

PoissonReconProperties::PoissonReconProperties()
  : mDepth(default_poisson_recon_depth),
    mSolveDepth(default_poisson_recon_solve_depth),
    mBoundaryType(default_poisson_recon_boundary_type)
{
}

PoissonReconProperties::~PoissonReconProperties() = default;

auto PoissonReconProperties::depth() const -> int
{
    return mDepth;
}

auto PoissonReconProperties::solveDepth() const -> int
{
    return mSolveDepth;
}

auto PoissonReconProperties::boundaryType() const -> BoundaryType
{
    return mBoundaryType;
}

auto PoissonReconProperties::boundaryTypeAsText() const -> QString
{
    QString boundary_type;

    switch (mBoundaryType) {
    case BoundaryType::free:
        boundary_type = "Free";
        break;
    case BoundaryType::dirichlet:
        boundary_type = "Dirichlet";
        break;
    case BoundaryType::neumann:
        boundary_type = "Neumann";
        break;
    }

    return boundary_type;
}

void PoissonReconProperties::setDepth(int depth)
{
    mDepth = depth;
}

void PoissonReconProperties::setSolveDepth(int solveDepth)
{
    mSolveDepth = solveDepth;
}

void PoissonReconProperties::setBoundaryType(BoundaryType boundaryType)
{
    mBoundaryType = boundaryType;
}


void PoissonReconProperties::clear()
{
    mDepth = default_poisson_recon_depth;
    mSolveDepth = default_poisson_recon_solve_depth;
    mBoundaryType = default_poisson_recon_boundary_type;
}



/* PoissonReconTask */


PoissonReconTask::PoissonReconTask(tl::Path input,
                                   tl::Path output)
  : mInput(std::move(input)),
    mOutput(std::move(output))
{

}

PoissonReconTask::~PoissonReconTask() = default;

void PoissonReconTask::poissonRecon(const tl::Path &app_path) const
{

    using Kernel = Simple_cartesian<double>;
    using Point = Kernel::Point_3;
    using Point_set = Point_set_3<Point>;

    try {

        if (mOutput.exists()) tl::Path::removeFile(mOutput);

        auto input = mInput;

        /// Para Law-Game
        std::ifstream in(mInput.toString(), std::ios::binary);
        Point_set pts;
        in >> pts;
        bool has_scalar_label = pts.has_property_map<float>("scalar_label");
        if (has_scalar_label) {
            pts.remove_property_map(pts.property_map<float>("scalar_label").first);

            auto copy_point_cloud = mInput;
            copy_point_cloud.replaceBaseName("temp");
            input = copy_point_cloud;
            std::ofstream out(input.toString(), std::ios::binary);
            CGAL::IO::set_binary_mode(out);
            CGAL::IO::write_PLY(out, pts);
        }

        std::string boundary_type;

        switch (boundaryType()) {
        case BoundaryType::free:
            boundary_type = "1";
            break;
        case BoundaryType::dirichlet:
            boundary_type = "2";
            break;
        case BoundaryType::neumann:
            boundary_type = "3";
            break;
        }

        std::string cmd("\"");
        cmd.append(app_path.parentPath().toString());
        cmd.append("\\PoissonRecon.exe\" ");
        cmd.append("--in \"").append(input.toString());
        cmd.append("\" --out \"").append(mOutput.toString());
        cmd.append("\" --depth ").append(std::to_string(depth()));
        cmd.append(" --solveDepth ").append(std::to_string(solveDepth()));
        cmd.append(" --bType ").append(boundary_type);
        cmd.append(" --density ");
        cmd.append(" --samplesPerNode 5");
        //cmd.append(" --samplesPerNode < minimum number of samples per node >= 1.500000]
        //[--pointWeight < interpolation weight >= 2.000e+00 * <b - spline degree>]
        tl::Message::info("Process: {}", cmd);

        tl::Process process(cmd);

        process.run();

        if (has_scalar_label) {
            tl::Path::removeFile(input);
        }

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "Poisson Reconstruction error. Process error.");

        TL_ASSERT(mOutput.exists(), "Poisson Reconstruction error. The mesh has not been generated.");


    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Mesh error");
    }
}

void PoissonReconTask::surfaceTrimmer(const tl::Path &app_path) const
{
    try {

        std::string cmd("\"");
        cmd.append(app_path.parentPath().toString());
        cmd.append("\\SurfaceTrimmer.exe\" ");
        cmd.append("--in \"").append(mOutput.toString());
        cmd.append("\" --out \"").append(mOutput.toString());
        cmd.append("\" --trim 7");

        tl::Process process(cmd);

        process.run();

        TL_ASSERT(process.status() == tl::Process::Status::finalized, "Surface Trimmer error");

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Mesh error");
    }
}

void PoissonReconTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Path app_path = tl::App::instance().path();


        poissonRecon(app_path);

        surfaceTrimmer(app_path);

        mReport.time = this->time();

        tl::Message::success("Poisson reconstruction finished in {:.2} minutes", mReport.time / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Poisson reconstruction error");
    }

}

auto PoissonReconTask::report() const -> MeshReport
{
    return mReport;
}

} // namespace graphos
