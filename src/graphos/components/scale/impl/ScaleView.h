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


#ifndef GRAPHOS_SCALE_VIEW_H
#define GRAPHOS_SCALE_VIEW_H

#include "graphos/components/scale/ScaleView.h"

class QDialogButtonBox;
class QLabel;
class QDoubleSpinBox;
class QPushButton;


namespace graphos
{

class ScaleViewImp
  : public ScaleView
{

  Q_OBJECT

public:

  ScaleViewImp(QWidget *parent = nullptr);
  ~ScaleViewImp() override;

// ScaleView

public:

  double distance() const override;
  double distanceReal() const override;

public slots:
  
  void setDistance(double distance) override;

// DialogView

private:

  void initUI();
  void initSignalAndSlots();

public slots:

  void clear();

private slots:

  void update();
  void retranslate();

protected:

  QLabel *mLabelDistance;
  QDoubleSpinBox *mDoubleSpinBoxDistance;
  QPushButton *mPushButtonDistance;
  QLabel *mLabelDistanceReal;
  QDoubleSpinBox *mDoubleSpinBoxDistanceReal;

  QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_SCALE_VIEW_H
