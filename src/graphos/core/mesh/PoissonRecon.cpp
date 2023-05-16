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

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>
#include <tidop/core/app.h>
#include <tidop/core/path.h>
#include <tidop/core/progress.h>
#include <tidop/core/chrono.h>

namespace graphos
{


  /// valores por defecto

  constexpr auto DefaultPoissonReconDepth = 11;
  constexpr auto DefaultPoissonReconSolveDepth = 0;
  constexpr auto DefaultPoissonReconBoundaryType = "Neumann";
  constexpr auto DefaultPoissonReconWidth = 0;
  constexpr auto DefaultPoissonReconFullDepth = 5;


/* PoissonReconParameters */

PoissonReconParameters::PoissonReconParameters()
  : mDepth(DefaultPoissonReconDepth),
    mSolveDepth(DefaultPoissonReconSolveDepth),
    mBoundaryType(DefaultPoissonReconBoundaryType),
    mWidth(DefaultPoissonReconWidth),
    mFullDepth(DefaultPoissonReconFullDepth)
{
}

PoissonReconParameters::~PoissonReconParameters()
{
}

int PoissonReconParameters::depth() const
{
  return mDepth;
}

int PoissonReconParameters::solveDepth() const
{
  return mSolveDepth;
}

QString PoissonReconParameters::boundaryType() const
{
  return mBoundaryType;
}

int PoissonReconParameters::width() const
{
  return mWidth;
}

int PoissonReconParameters::fullDepth() const
{
  return mFullDepth;
}

void PoissonReconParameters::setDepth(int Depth) 
{
  mDepth = Depth;
}

void PoissonReconParameters::setSolveDepth(int SolveDepth) 
{
  mSolveDepth = SolveDepth;
}

void PoissonReconParameters::setBoundaryType(const QString &BoundaryType) 
{
  mBoundaryType = BoundaryType;
}

void PoissonReconParameters::setWidth(int width) 
{
  mWidth = width;
}

void PoissonReconParameters::setFullDepth(int FullDepth) 
{
  mFullDepth = FullDepth;
}

void PoissonReconParameters::clear()
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
    PoissonReconParameters(),
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

    tl::Chrono chrono("Poisson reconstruction finished");
    chrono.run();

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
      cmd.append(" --width ").append(std::to_string(width()));
      cmd.append(" --bType ").append(b_type);
      cmd.append(" --density ");
      cmd.append(" --samplesPerNode 5");
      //cmd.append(" --samplesPerNode < minimum number of samples per node >= 1.500000]
      //[--pointWeight < interpolation weight >= 2.000e+00 * <b - spline degree>]
      msgInfo("Process: %s", cmd.c_str());

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

    chrono.stop();

    if(progressBar) (*progressBar)();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Load images error");
  }

}


} // namespace graphos
