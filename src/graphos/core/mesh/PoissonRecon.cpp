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

#include <tidop/core/exception.h>
#include <tidop/core/app.h>
#include <tidop/core/path.h>
#include <tidop/core/progress.h>
#include <tidop/core/chrono.h>

namespace graphos
{


/// valores por defecto

constexpr auto DefaultPoissonReconDepth = 11;
constexpr auto DefaultPoissonReconSolveDepth = 10;
constexpr auto DefaultPoissonReconBoundaryType = "Neumann";
constexpr auto DefaultPoissonReconWidth = 0;
constexpr auto DefaultPoissonReconFullDepth = 5;


/* PoissonReconProperties */

PoissonReconProperties::PoissonReconProperties()
  : mDepth(DefaultPoissonReconDepth),
    mSolveDepth(DefaultPoissonReconSolveDepth),
    mBoundaryType(DefaultPoissonReconBoundaryType),
    mWidth(DefaultPoissonReconWidth),
    mFullDepth(DefaultPoissonReconFullDepth)
{
}

PoissonReconProperties::~PoissonReconProperties()
{
}

int PoissonReconProperties::depth() const
{
    return mDepth;
}

int PoissonReconProperties::solveDepth() const
{
    return mSolveDepth;
}

QString PoissonReconProperties::boundaryType() const
{
    return mBoundaryType;
}

int PoissonReconProperties::width() const
{
    return mWidth;
}

int PoissonReconProperties::fullDepth() const
{
    return mFullDepth;
}

void PoissonReconProperties::setDepth(int Depth)
{
    mDepth = Depth;
}

void PoissonReconProperties::setSolveDepth(int SolveDepth)
{
    mSolveDepth = SolveDepth;
}

void PoissonReconProperties::setBoundaryType(const QString &BoundaryType)
{
    mBoundaryType = BoundaryType;
}

void PoissonReconProperties::setWidth(int width)
{
    mWidth = width;
}

void PoissonReconProperties::setFullDepth(int FullDepth)
{
    mFullDepth = FullDepth;
}

void PoissonReconProperties::clear()
{
    mDepth = DefaultPoissonReconDepth;
    mSolveDepth = DefaultPoissonReconSolveDepth;
    mBoundaryType = DefaultPoissonReconBoundaryType;
    mWidth = DefaultPoissonReconWidth;
    mFullDepth = DefaultPoissonReconFullDepth;
}


/* PoissonReconTask */


PoissonReconTask::PoissonReconTask(const tl::Path &input,
                                   const tl::Path &output)
  : tl::TaskBase(),
    PoissonReconProperties(),
    mInput(input),
    mOutput(output)
{

}

PoissonReconTask::~PoissonReconTask()
{

}

void PoissonReconTask::execute(tl::Progress *progressBar)
{

    try {

        tl::Path app_path = tl::App::instance().path();

        // Poisson Reconstruction
        {

            std::string b_type;

            if (boundaryType() == "free") {
                b_type = "1";
            } else if (boundaryType() == "Dirichlet") {
                b_type = "2";
            } else {
                b_type = "3";
            }

            std::string cmd("\"");
            cmd.append(app_path.parentPath().toString());
            cmd.append("\\PoissonRecon.exe\" ");
            cmd.append("--in \"").append(mInput.toString());
            cmd.append("\" --out \"").append(mOutput.toString());
            cmd.append("\" --depth ").append(std::to_string(depth()));
            cmd.append(" --solveDepth ").append(std::to_string(solveDepth()));
            //cmd.append(" --width ").append(std::to_string(width()));
            cmd.append(" --bType ").append(b_type);
            cmd.append(" --density ");
            cmd.append(" --samplesPerNode 5");
            //cmd.append(" --samplesPerNode < minimum number of samples per node >= 1.500000]
            //[--pointWeight < interpolation weight >= 2.000e+00 * <b - spline degree>]
            tl::Message::info("Process: {}", cmd);

            tl::Process process(cmd);

            process.run();

            TL_ASSERT(process.status() == tl::Process::Status::finalized, "Poisson Reconstruction error");
        }

        // Surface Trimmer
        {
            std::string cmd("\"");
            cmd.append(app_path.parentPath().toString());
            cmd.append("\\SurfaceTrimmer.exe\" ");
            cmd.append("--in \"").append(mOutput.toString());
            cmd.append("\" --out \"").append(mOutput.toString());
            cmd.append("\" --trim 7");

            tl::Process process(cmd);

            process.run();

            TL_ASSERT(process.status() == tl::Process::Status::finalized, "Surface Trimmer error");

        }

        mReport.time = this->time();

        tl::Message::success("Poisson reconstruction finished in {:.2} minutes", mReport.time / 60.);

        if (progressBar) (*progressBar)();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Load images error");
    }

}

auto PoissonReconTask::report() const -> MeshReport
{
    return mReport;
}

} // namespace graphos
