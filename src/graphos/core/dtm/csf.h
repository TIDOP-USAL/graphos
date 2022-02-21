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

#ifndef GRAPHOS_CORE_DTM_CSF_H
#define GRAPHOS_CORE_DTM_CSF_H

#include "graphos/graphos_global.h"

#include <map>
#include <memory>

#include <QString>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>


namespace graphos
{

namespace internal
{
class Csf;
}

class GRAPHOS_EXPORT Csf
{

public:

  Csf();
  ~Csf();

  bool sloopSmooth() const;
  void setSloopSmooth(bool sloopSmooth);
  double timeStep() const;
  void setTimeStep(double timeStep);
  double classThreshold() const;
  void setClassThreshold(double classThreshold);
  double clothResolution() const;
  void setClothResolution(double clothResolution);
  int rigidness() const;
  void setRigidness(int rigidness);
  int iterations() const;
  void setIterations(int iterations);

  void filter(const std::string &pointCloud, 
              const std::string &ground, 
              const std::string &outGround);

  void reset();

private:

  std::unique_ptr<internal::Csf> mCSF;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DTM_CSF_H
