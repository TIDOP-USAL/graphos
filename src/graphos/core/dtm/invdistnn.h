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

#ifndef GRAPHOS_CORE_DTM_INVDISTNN_H
#define GRAPHOS_CORE_DTM_INVDISTNN_H

#include "graphos/core/dtm/dtm.h"

namespace graphos
{

class DtmInvDistNNProperties
  : public DtmInvDistNN
{

public:

  DtmInvDistNNProperties();
  ~DtmInvDistNNProperties();

// DtmInvDistNN interface

public:

  double power() const override;
  double smoothing() const override;
  double radius() const override;
  int maxPoints() const override;
  int minPoints() const override;

  void setPower(double power) override;
  void setSmoothing(double smoothing) override;
  void setRadius(double radius) override;
  void setMaxPoints(int maxPoints) override;
  void setMinPoints(int minPoints) override;

// Dtm interface

public:

  std::string name() const override final;
  void reset() override;

protected:

  double mPower;
  double mSmoothing;
  double mRadius;
  int mMaxPoints;
  int mMinPoints;

};




class DtmInvDistNNAlgorithm
  : public DtmInvDistNNProperties,
    public DtmAlgorithm
{

public:

  DtmInvDistNNAlgorithm();
  DtmInvDistNNAlgorithm(double power,
                        double smoothing,
                        double radius,
                        int maxPoints,
                        int minPoints);
  ~DtmInvDistNNAlgorithm();

// DtmAlgorithm interface

public:

  bool run(const std::string &pointCloud,
           const std::string &dtmFile,
           const tl::Size<int> &size) override;

  bool run(const std::string &pointCloud,
           const std::string &dtmFile,
           const tl::BoundingBox<tl::Point3<double>> &bbox,
           double gsd) override;

};


} // namespace graphos

#endif // GRAPHOS_CORE_DTM_INVDISTNN_H