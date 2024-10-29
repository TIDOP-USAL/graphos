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

#ifndef GRAPHOS_SIFT_WIDGET_H
#define GRAPHOS_SIFT_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{


class SiftWidget
    : public GraphosWidgetView
{
    Q_OBJECT

public:

    SiftWidget(QWidget *parent = nullptr);
    ~SiftWidget() override;

public:

    auto featuresNumber() const -> int;
    auto octaveLayers() const -> int;
    auto contrastThreshold() const -> double;
    auto constrastThresholdAuto() const -> bool;
    auto edgeThreshold() const -> double;
    //auto domainSizePooling() const -> bool;

public slots:

    void setFeaturesNumber(int featuresNumber);
    void setOctaveLayers(int octaveLayers);
    void setContrastThreshold(double contrastThreshold);
    void setContrastThresholdAuto(bool active);
    void setEdgeThreshold(double edgeThreshold);
    //void setDomainSizePooling(bool domainSizePooling);

signals:

    void featuresNumberChange(int);
    void octaveLayersChange(int);
    void contrastThresholdAutoChange(bool);
    void contrastThresholdChange(double);
    void edgeThresholdChange(double);
    //void domainSizePooling(bool);

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
    QLabel *mLabelFeaturesNumber;
    QSpinBox *mFeaturesNumber;
    QLabel *mLabelOctaveLayers;
    QSpinBox *mOctaveLayers;
    QCheckBox *mContrastThresholdAuto;
    QLabel *mLabelContrastThreshold;
    QDoubleSpinBox *mContrastThreshold;
    QLabel *mLabelEdgeThreshold;
    QDoubleSpinBox *mEdgeThreshold;
    //QCheckBox *mDomainSizePooling;

};

} // namespace graphos

#endif // GRAPHOS_SIFT_WIDGET_H
