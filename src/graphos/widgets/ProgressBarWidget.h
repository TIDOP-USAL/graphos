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

#ifndef GRAPHOS_PROGRESS_BAR_WIDGET_H
#define GRAPHOS_PROGRESS_BAR_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"


class QProgressBar;
//class QPushButton;
class QAction;

namespace graphos
{


class ProgressBarWidget
    : public GraphosWidgetView
{
    Q_OBJECT

public:

    ProgressBarWidget(QWidget *parent = nullptr);
    ~ProgressBarWidget() override;

public slots:

    void setRange(int min, int max);
    void setValue(int value);

protected slots:

    void onPushButtonCancelClicked();

signals:

    void cancel();
    void maximized();

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

    QProgressBar *mProgressBar;
    QAction *mActionCancel;
    QAction *mActionMaximize;
};

} // namespace graphos

#endif // GRAPHOS_PROGRESS_BAR_WIDGET_H
