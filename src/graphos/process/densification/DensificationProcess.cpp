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

#include "graphos/process/densification/DensificationProcess.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

DensificationProcess::DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                                           const QString &reconstructionPath,
                                           const QString &outputPath)
  : ProcessConcurrent(),
    mDensifier(densifier),
    mReconstructionPath(reconstructionPath),
    mOutputPat(outputPath)
{

}

void DensificationProcess::run()
{
  try {

    msgInfo("Starting Densification");

    tl::Chrono chrono;
    chrono.run();

    mDensifier->undistort(mReconstructionPath, mOutputPat);

    mDensifier->densify(mOutputPat);

    double time = chrono.stop();
    msgInfo("[Time: %f seconds]", time);

    emit densificationFinished();

    msgInfo("Densification finished.");

  } catch (std::exception &e) {
    emit error(0, "Densification error");
    msgError(e.what());
  } catch (...) {
    emit error(0, "Densification error");
    msgError("Unhandled exception");
  }
}

QString DensificationProcess::outputPat() const
{
  return mOutputPat;
}

void DensificationProcess::setOutputPat(const QString &outputPat)
{
  mOutputPat = outputPat;
}

QString DensificationProcess::reconstructionPath() const
{
  return mReconstructionPath;
}

void DensificationProcess::setReconstructionPath(const QString &reconstructionPath)
{
  mReconstructionPath = reconstructionPath;
}

} // graphos
