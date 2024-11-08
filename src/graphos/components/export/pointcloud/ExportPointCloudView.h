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

#ifndef GRAPHOS_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H
#define GRAPHOS_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"

#include <QFileDialog>

namespace graphos
{

/*!
 * \brief ExportPointCloudView interface
 */
class ExportPointCloudView
  : public DialogView
{
    Q_OBJECT

public:

    ExportPointCloudView(QWidget *parent) : DialogView(parent) {}
    ~ExportPointCloudView() override = default;

    virtual auto format() -> QString = 0;
    virtual void addFormat(QWidget *format) = 0;
    virtual void setFormat(const QString &format) = 0;

    virtual auto crs() const -> QString = 0;
    virtual void setCrs(const QString &crs) = 0;

signals:

    void crs_change();
    void select_crs();
    void run();

};

} // namespace graphos

#endif // GRAPHOS_EXPORT_POINT_CLOUD_VIEW_INTERFACE_H