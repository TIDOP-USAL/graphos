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

#ifndef GRAPHOS_ORIENTATION_VIEW_INTERFACE_H
#define GRAPHOS_ORIENTATION_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"


namespace graphos
{

class OrientationView
  : public DialogView
{

  Q_OBJECT

public:

  OrientationView(QWidget *parent = nullptr) : DialogView(parent) {}
  ~OrientationView() override = default;

  //virtual bool refineFocal() const = 0;
  //virtual bool refinePrincipalPoint() const = 0;
  //virtual bool refineDistortionParameters() const = 0;
  //virtual bool refinePoses() const = 0;
  virtual bool fixCalibration() const = 0;
  virtual bool isEnabledCalibration() const = 0;
  virtual bool absoluteOrientation() const = 0;
  virtual bool isEnabledAbsoluteOrientation() const = 0;
  virtual bool fixPoses() const = 0;
  virtual bool isEnabledPoses() const = 0;

public slots:

  //virtual void setRefineFocal(bool active) = 0;
  //virtual void setRefinePrincipalPoint(bool active) = 0;
  //virtual void setRefineDistortionParameters(bool active) = 0;
  //virtual void setRefinePoses(bool active) = 0;
  virtual void setCalibration(bool active) = 0;
  virtual void enabledCalibration(bool enabled) = 0;
  virtual void setAbsoluteOrientation(bool active) = 0;
  virtual void enabledAbsoluteOrientation(bool enabled) = 0;
  virtual void setPoses(bool active) = 0;
  virtual void enabledPoses(bool enabled) = 0;

signals:

  void run();
  //void refineFocal(bool);
  //void refinePrincipalPoint(bool);
  //void refineDistortionParametersChange(bool);
  //void refinePosesChange(bool);
  void calibrationChange(bool);
  void absoluteOrientationChange(bool);
  void posesChange(bool);
};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
