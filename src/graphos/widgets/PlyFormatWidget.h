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

#ifndef GRAPHOS_POINT_CLOUD_PLY_FORMAT_WIDGET_H
#define GRAPHOS_POINT_CLOUD_PLY_FORMAT_WIDGET_H

#include <tidop/core/flags.h>

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QCheckBox;
class QRadioButton;

namespace graphos
{


class PlyFormatWidget
  : public GraphosWidgetView
{

    Q_OBJECT

public:

    enum class Format
    {
        binary = (1 << 0),
        text = (1 << 1),
    };

public:

    PlyFormatWidget(QWidget *parent = nullptr);
    ~PlyFormatWidget() override = default;

public:

    auto format() const -> Format;
    void setFormat(Format format);
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

    QGroupBox *mGroupBoxFormat;
    QRadioButton *mRadioButtonBinary;
    QRadioButton *mRadioButtonText;
    QCheckBox *mCheckBoxColors;
    QCheckBox *mCheckBoxNormals;

private:

    tl::EnumFlags<Format> mFormat;
};
ALLOW_BITWISE_FLAG_OPERATIONS(PlyFormatWidget::Format)

} // namespace graphos


#endif // GRAPHOS_POINT_CLOUD_PLY_FORMAT_WIDGET_H
