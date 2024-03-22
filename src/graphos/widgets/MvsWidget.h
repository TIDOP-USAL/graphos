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
class QComboBox;

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

    auto quality() const -> int;
    //int resolutionLevel() const;
    //int minResolution() const;
    //int maxResolution() const;
    auto numberViews() const -> int;
    auto numberViewsFuse() const -> int;
    auto estimateColors() const -> bool;
    auto estimateNormals() const -> bool;

signals:

    void qualityChanged(int);
    //void resolutionLevelChanged(int);
    //void minResolutionChanged(int);
    //void maxResolutionChanged(int);
    void numberViewsChanged(int);
    void numberViewsFuseChanged(int);

public slots:

    void setQuality(int quality);
    //void setResolutionLevel(int resolutionLevel);
    //void setMinResolution(int minResolution);
    //void setMaxResolution(int maxResolution);
    void setNumberViews(int numberViews);
    void setNumberViewsFuse(int numberViewsFuse);
    void setEstimateColors(bool estimateColors);
    void setEstimateNormals(bool estimateNormals);

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
    QLabel *mLabelQuality;
    QComboBox *mComboBoxQuality;
    //QLabel *mLabelResolutionLevel;
    //QSpinBox *mSpinBoxResolutionLevel;
    //QLabel *mLabelMinResolution;
    //QSpinBox *mSpinBoxMinResolution;
    //QLabel *mLabelMaxResolution;
    //QSpinBox *mSpinBoxMaxResolution;
    QLabel *mLabelNumberViews;
    QSpinBox *mSpinBoxNumberViews;
    QLabel *mLabelNumberViewsFuse;
    QSpinBox *mSpinBoxNumberViewsFuse;
    QCheckBox *mCheckBoxEstimateColors;
    QCheckBox *mCheckBoxEstimateNormals;

    //--fusion-mode arg (=0)                depth-maps fusion mode (-2 - fuse
    //                                    disparity-maps, -1 - export
    //                                    disparity-maps only, 0 - depth-maps &
    //                                    fusion, 1 - export depth-maps only)
    //--postprocess-dmaps arg (=7)          flags used to filter the depth-maps
    //                                    after estimation (0 - disabled, 1 -
    //                                    remove-speckles, 2 - fill-gaps, 4 -
    //                                    adjust-filter)

};

} // namespace graphos

#endif // GRAPHOS_MVS_WIDGET_H
