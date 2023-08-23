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
    ~FeatureMatchingWidget() override = default;

    virtual bool crossCheck() const = 0;
    virtual double ratio() const = 0;
    virtual double distance() const = 0;
    //virtual int maxMatches() const = 0;
    virtual double maxError() const = 0;
    virtual double confidence() const = 0;

signals:

    void crossCheckChanged(bool);
    void ratioChanged(double);
    void distanceChanged(double);
    //void maxMachesChanged(int);
    void maxErrorChanged(double);
    void confidenceChanged(double);

public slots:

    virtual void enableCrossCheck(bool enable = true) = 0;
    virtual void setRatio(double ratio) = 0;
    virtual void setDistance(double distance) = 0;
    //virtual void setMaxMatches(int maxMatches) = 0;
    virtual void setMaxError(double error) = 0;
    virtual void setConfidence(double confidence) = 0;
};


class FeatureMatchingWidgetImp
    : public FeatureMatchingWidget
{

    Q_OBJECT

public:

    FeatureMatchingWidgetImp(QWidget *parent = nullptr);
    ~FeatureMatchingWidgetImp() override;

    // FeatureMatchingWidget interface

public:

    bool crossCheck() const override;
    double ratio() const override;
    double distance() const override;
    //int maxMatches() const override;
    double maxError() const override;
    double confidence() const override;

public slots:

    void enableCrossCheck(bool enable) override;
    void setRatio(double ratio) override;
    void setDistance(double distance) override;
    //void setMaxMatches(int maxMatches) override;
    void setMaxError(double error) override;
    void setConfidence(double confidence) override;

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
