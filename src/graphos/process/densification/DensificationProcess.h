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

#ifndef GRAPHOS_DENSIFICATION_PROCESS_H
#define GRAPHOS_DENSIFICATION_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/densification/densification.h"



namespace graphos
{

class GRAPHOS_EXPORT DensificationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  DensificationProcess(const std::shared_ptr<Densifier> &densifier,
                       const QString &reconstructionPath,
                       const QString &outputPath);
  ~DensificationProcess() override = default;

  QString reconstructionPath() const;
  void setReconstructionPath(const QString &reconstructionPath);


  QString outputPat() const;
  void setOutputPat(const QString &outputPat);

signals:

  void densificationFinished();

  // ProcessConcurrent interface

protected:

  void run() override;

private:

  std::shared_ptr<Densifier> mDensifier;
  QString mReconstructionPath;
  QString mOutputPat;
};

} // namespace graphos

#endif // GRAPHOS_DENSIFICATION_PROCESS_H
