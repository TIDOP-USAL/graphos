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

#ifndef GRAPHOS_EXPORT_POINT_CLOUD_VIEW_H
#define GRAPHOS_EXPORT_POINT_CLOUD_VIEW_H

#include "graphos/components/export/pointcloud/ExportPointCloudView.h"

class QGridLayout;
class QDialogButtonBox;
class QLabel;
class QLineEdit;

namespace graphos
{

class ExportPointCloudViewImp
  : public ExportPointCloudView
{

    Q_OBJECT

public:

    explicit ExportPointCloudViewImp(QWidget *parent = nullptr);
    ~ExportPointCloudViewImp() override = default;

// ExportPointCloudView interface

public:

    auto format() -> QString override;
    void addFormat(QWidget *format) override;
    void setFormat(const QString &format) override;

    auto crs() const -> QString override;
    void setCrs(const QString &crs) override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

private:

    QLabel *mLabelCRS;
    QLineEdit *mLineEditCRS;
    QPushButton *mPushButtonCRS;
    QGridLayout *mGridLayoutFormat;
    QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_EXPORT_POINT_CLOUD_VIEW_H
