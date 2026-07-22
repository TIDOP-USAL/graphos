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

#ifndef GRAPHOS_POINT_CLOUD_LAS_FORMAT_WIDGET_H
#define GRAPHOS_POINT_CLOUD_LAS_FORMAT_WIDGET_H

#include <tidop/core/base/flags.h>

#include "graphos/widgets/GraphosWidget.h"


class QLabel;
class QCheckBox;

namespace graphos
{


class LasFormatWidget
  : public GraphosWidgetView
{

    Q_OBJECT

public:

    LasFormatWidget(QWidget *parent = nullptr);
    ~LasFormatWidget() override = default;

public:

    void enableExportColors(bool active);
    auto isExportColorsEnabled() const -> bool;
    void enableExportNormals(bool active);
    auto isExportNormalsEnabled() const -> bool;

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

    QCheckBox *mCheckBoxColors;
    QCheckBox *mCheckBoxNormals;

};

} // namespace graphos


#endif // GRAPHOS_POINT_CLOUD_LAS_FORMAT_WIDGET_H
