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

#include "DensificationProcess.h"

#include "graphos/core/densification/densification.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

namespace graphos
{

DensificationProcess::DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                                           const QString &reconstructionPath,
                                           const QString &outputPath)
  : tl::TaskBase(),
    mDensifier(densifier),
    mReconstructionPath(reconstructionPath),
    mOutputPat(outputPath)
{

}

void DensificationProcess::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Densification finished");
    chrono.run();

    mDensifier->undistort(mReconstructionPath, mOutputPat);

    if(status() == tl::Task::Status::stopping) return;

    mDensifier->densify(mOutputPat);

    chrono.stop();

    emit densificationFinished();
    if(progressBar) (*progressBar)();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Densification error");
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
