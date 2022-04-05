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

#ifndef GRAPHOS_MVS_WIDGET_H
#define GRAPHOS_MVS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

class MvsWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  MvsWidget(QWidget *parent = nullptr);
  ~MvsWidget() override;

public:

  int resolutionLevel() const;
  int minResolution() const;
  int maxResolution() const;
  int numberViewsFuse() const;
  
signals:

  void resolutionLevelChanged(int);
  void minResolutionChanged(int);
  void maxResolutionChanged(int);
  void numberViewsFuseChanged(int);
  
public slots:

  void setResolutionLevel(int resolutionLevel);
  void setMinResolution(int minResolution);
  void setMaxResolution(int maxResolution);
  void setNumberViewsFuse(int numberViewsFuse);

// GraphosWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

protected:
  
  QGroupBox *mGroupBox;
  QLabel *mLabelResolutionLevel;
  QSpinBox *mSpinBoxResolutionLevel;
  QLabel *mLabelMinResolution;
  QSpinBox *mSpinBoxMinResolution;
  QLabel *mLabelMaxResolution;
  QSpinBox *mSpinBoxMaxResolution;
  QLabel *mLabelNumberViewsFuse;
  QSpinBox *mSpinBoxNumberViewsFuse;

};

} // namespace graphos

#endif // GRAPHOS_MVS_WIDGET_H
