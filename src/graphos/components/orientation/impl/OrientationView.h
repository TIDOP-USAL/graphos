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

#ifndef GRAPHOS_ORIENTATION_VIEW_H
#define GRAPHOS_ORIENTATION_VIEW_H

#include "graphos/components/orientation/OrientationView.h"

class QCheckBox;
class QDialogButtonBox;

namespace graphos
{

class OrientationViewImp
  : public OrientationView
{

public:

  OrientationViewImp(QWidget *parent = nullptr);
  ~OrientationViewImp() override = default;

// OrientationView interface

public:

  bool fixCalibration() const override;
  bool isEnabledCalibration() const override;
  bool absoluteOrientation() const override;
  bool isEnabledAbsoluteOrientation() const override;
  bool fixPoses() const override;
  bool isEnabledPoses() const override;

public slots:

  void setCalibration(bool active) override;
  void enabledCalibration(bool enabled) override;
  void setAbsoluteOrientation(bool active) override;
  void enabledAbsoluteOrientation(bool enabled) override;
  void setPoses(bool active) override;
  void enabledPoses(bool enabled) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QCheckBox *mCheckBoxFixCalibration;
  QCheckBox *mCheckBoxFixPoses;
  QCheckBox *mCheckBoxAbsoluteOrientation;
  QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_H
