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

#ifndef GRAPHOS_TIFF_FORMAT_WIDGET_H
#define GRAPHOS_TIFF_FORMAT_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QDialogButtonBox;
class QCheckBox;
class QComboBox;
class QLabel;
class QSpinBox;

namespace graphos
{
	
class TiffFormatWidget
  : public GraphosWidgetView
{
    Q_OBJECT

public:

    TiffFormatWidget(QWidget *parent = nullptr);
    ~TiffFormatWidget() override;
	
public:

    QString bigTiff() const;
    QString compress() const;
    bool tiled() const;
    int blockXSize() const;
    int blockYSize() const;
    int jpegQuality() const;
	
public slots:
  
    void setBigtiff(const QString& value);
    void setCompress(const QString& value);
    void setTiled(bool value);
    void setBlockxsize(int value);
    void setBlockysize(int value);
    void setJpegquality(int value);

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

    QLabel *mLabelBigtiff;
    QLabel *mLabelCompress;
    QLabel *mLabelBlockxsize;
    QLabel *mLabelBlockysize;
    QLabel *mLabelJpegquality;
    QComboBox *mBigtiff;
    QComboBox *mCompress;
    QCheckBox *mTiled;
    QSpinBox *mBlockxsize;
    QSpinBox *mBlockysize;
    QSpinBox *mJpegquality;
    QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_TIFF_FORMAT_WIDGET_H
