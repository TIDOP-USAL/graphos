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

#ifndef GRAPHOS_DTM_PROCESS_H
#define GRAPHOS_DTM_PROCESS_H

#include "graphos/core/dtm/dtm.h"

#include <tidop/core/process.h>
#include <tidop/core/progress.h>
#include <tidop/geometry/entities/point.h>

#include <QObject>

namespace graphos
{

class DenseExport;

class GRAPHOS_EXPORT DtmProcess
  : public QObject,
    public tl::ProcessBase
{

  Q_OBJECT

public:

  DtmProcess(const std::shared_ptr<DtmAlgorithm> &dtmAlgorithm,
             const QString &pointCloud,
             const tl::Point3<double> &offset,
             const QString &dtmFile,
             double gsd,
             bool dsm,
             const QString &crs);
  ~DtmProcess() override = default;

signals:

  //void dtmFinished();

// tl::ProcessBase interface

protected:

  void execute(tl::Progress *progressBar) override;

private:

  std::shared_ptr<DtmAlgorithm> mDtmAlgorithm;
  QString mPointCloud;
  tl::Point3<double> mOffset;
  QString mDtmFile;
  double mGSD;
  bool mDSM;
  QString mCrs;
};

} // namespace graphos

#endif // GRAPHOS_DTM_PROCESS_H
