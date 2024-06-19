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

#ifndef FEATUREMATCHINGWIDGET_H
#define FEATUREMATCHINGWIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{


class FeatureMatchingWidget
  : public GraphosWidgetView
{

    Q_OBJECT

public:

    FeatureMatchingWidget(QWidget *parent = nullptr);
    ~FeatureMatchingWidget() override;

public:

    auto crossCheck() const -> bool;
    auto ratio() const -> double;
    auto distance() const -> double;
    auto maxError() const -> double;
    auto confidence() const -> double;

public slots:

    void enableCrossCheck(bool enable);
    void setRatio(double ratio);
    void setDistance(double distance);
    void setMaxError(double error);
    void setConfidence(double confidence);

signals:

    void crossCheckChanged(bool);
    void ratioChanged(double);
    void distanceChanged(double);
    void maxErrorChanged(double);
    void confidenceChanged(double);

// GraphosWidgetView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

protected:

    QGroupBox *mGroupBox;
    QCheckBox *mCheckBoxCrossCheck;
    QLabel *mLabelRatio;
    QDoubleSpinBox *mDoubleSpinBoxRatio;
    QLabel *mLabelDistance;
    QDoubleSpinBox *mDoubleSpinBoxDistance;
    QLabel *mLabelMaxError;
    QDoubleSpinBox *mDoubleSpinBoxMaxError;
    QLabel *mLabelConfidence;
    QDoubleSpinBox *mDoubleSpinBoxConfidence;

};

} // namespace graphos

#endif // FEATUREMATCHINGWIDGET_H
