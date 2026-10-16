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

#pragma once

#include "graphos/widgets/GraphosWidget.h"

class QLabel;
class QComboBox;

namespace graphos
{


class SeamBasedBlendingMosaicWidget
    : public GraphosWidgetView
{
    Q_OBJECT


public:

    SeamBasedBlendingMosaicWidget(QWidget *parent = nullptr);
    ~SeamBasedBlendingMosaicWidget() override;

public:

    auto exposureCompensator() const -> QString;
    auto seamFinder() const -> QString;
    auto blenderType() const -> QString;

public slots:

    void setExposureCompensator(const QString &exposureCompensator);
    void setSeamFinder(const QString &seamFinder);
    void setBlenderType(const QString &blenderType);

signals:

    void compensatorChange(QString);
    void seamFinderChange(QString);
    void blenderTypeChange(QString);

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

    QLabel *mLabelCompensator;
    QComboBox *mExposureCompensator;
    QLabel *mLabelSeamFinder;
    QComboBox *mSeamFinder;
    QLabel *mLabelBlenderType;
    QComboBox *mBlenderType;

};

} // namespace graphos