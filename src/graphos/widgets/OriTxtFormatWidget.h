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

#ifndef GRAPHOS_ORIENTATION_FORMAT_TXT_WIDGET_H
#define GRAPHOS_ORIENTATION_FORMAT_TXT_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

namespace graphos
{



class OriTxtFormatWidget
  : public GraphosWidgetView
{

    Q_OBJECT

public:

    OriTxtFormatWidget(QWidget *parent = nullptr);
    ~OriTxtFormatWidget() override = default;

public:

    QString file() const;
    QString rotation() const;

public slots:

    void setFile(const QString &file);
    void setRotation(const QString &rotation);

protected slots:

    void onPushButtonSelectPath();

signals:

    void fileChanged(QString);
    void rotationChanged(QString);

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
    QLabel *mLabelRotation;
    QComboBox *mComboBoxRotation;
    QLabel *mLabelFile;
    QLineEdit *mLineEditFile;
    QPushButton *mPushButtonSelectPath;
};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_FORMAT_TXT_WIDGET_H
