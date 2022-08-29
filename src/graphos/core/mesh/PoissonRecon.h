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

#include <vector>
#include <map>
#include <string>

namespace tl
{
  class Process;
}

namespace graphos
{


class PoissonReconParameters
{ 

public:

  PoissonReconParameters();
  ~PoissonReconParameters();

  virtual int depth() const;
  virtual int solveDepth() const;
  virtual QString boundaryType() const;
  virtual int width() const;
  virtual int fullDepth() const;

  virtual void setDepth(int Depth);
  virtual void setSolveDepth(int SolveDepth);
  virtual void setBoundaryType(const QString &BoundaryType);
  virtual void setWidth(int width);
  virtual void setFullDepth(int FullDepth);

  void clear();

private:

  int mDepth;
  int mSolveDepth;
  QString mBoundaryType;
  int mWidth;
  int mFullDepth;

};


class PoissonReconTask
  : public tl::TaskBase,
    public PoissonReconParameters
{

public:

  PoissonReconTask(const tl::Path &input,
                   const tl::Path &output);
  ~PoissonReconTask();

  // tl::TaskBase interface

protected:

  void execute(tl::Progress *progressBar) override;

private:

  tl::Path mInput;
  tl::Path mOutput;
};



} // namespace graphos


#endif // GRAPHOS_POISSONRECON_ALGORITHM_H
