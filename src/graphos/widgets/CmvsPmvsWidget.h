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

#ifndef GRAPHOS_CMVS_PMVS_WIDGET_H
#define GRAPHOS_CMVS_PMVS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{


/*!
 * \brief Shading-Aware Multi-View Stereo Widget Interface
 */
class CmvsPmvsWidget
    : public GraphosWidgetView
{
    Q_OBJECT

public:

    CmvsPmvsWidget(QWidget *parent = nullptr);
    ~CmvsPmvsWidget() override;

public:

    bool useVisibilityInformation() const;
    int imagesPerCluster() const;
    int level() const;
    int cellSize() const;
    double threshold() const;
    int windowSize() const;
    int minimunImageNumber() const;

public slots:

    void setUseVisibilityInformation(bool useVisibilityInformation);
    void setImagesPerCluster(int imagesPerCluster);
    void setLevel(int level);
    void setCellSize(int cellSize);
    void setThreshold(double threshold);
    void setWindowSize(int windowSize);
    void setMinimunImageNumber(int minimunImageNumber);

signals:

    void useVisibilityInformationChanged(bool);
    void imagesPerClusterChanged(int);
    void levelChanged(int);
    void cellSizeChanged(int);
    void thresholdChanged(double);
    void windowSizeChanged(int);
    void minimunImageNumberChanged(int);

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
    QCheckBox *mCheckBoxUseVisibilityInformation;
    QLabel *mLabelImagesPerCluster;
    QSpinBox *mSpinBoxImagesPerCluster;
    QLabel *mLabelLevel;
    QSpinBox *mSpinBoxLevel;
    QLabel *mLabelCellSize;
    QSpinBox *mSpinBoxCellSize;
    QLabel *mLabelThreshold;
    QDoubleSpinBox *mDoubleSpinBoxThreshold;
    QLabel *mLabelWindowSize;
    QSpinBox *mSpinBoxWindowSize;
    QLabel *mLabelMinimunImageNumber;
    QSpinBox *mSpinBoxMinimunImageNumber;

};

} // namespace graphos

#endif // GRAPHOS_CMVS_PMVS_WIDGET_H
