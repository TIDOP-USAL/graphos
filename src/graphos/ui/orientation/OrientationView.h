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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
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

  virtual bool refinePrincipalPoint() const = 0;
  virtual bool absoluteOrientation() const = 0;
  virtual bool isEnabledAbsoluteOrientation() const = 0;

public slots:

  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setAbsoluteOrientation(bool active) = 0;
  virtual void enabledAbsoluteOrientation(bool enabled) = 0;

signals:

  void run();
  void refinePrincipalPoint(bool);
  void absoluteOrientationChange(bool);
  void enabledAbsoluteOrientationChange(bool);
};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
