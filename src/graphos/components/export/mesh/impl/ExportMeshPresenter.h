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

#ifndef GRAPHOS_EXPORT_MESH_PRESENTER_H
#define GRAPHOS_EXPORT_MESH_PRESENTER_H

#include "graphos/components/export/mesh/ExportMeshPresenter.h"

namespace graphos
{

class ExportMeshView;
class ExportMeshModel;
class AppStatus;

class ExportMeshPresenterImp
  : public ExportMeshPresenter
{

    Q_OBJECT

public:

    ExportMeshPresenterImp(ExportMeshView *view,
                           ExportMeshModel *model,
                           AppStatus *status);
    ~ExportMeshPresenterImp() override;

// ExportMeshPresenter interface

public slots:

    void exportMesh(const QString &file) override;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

private:

    ExportMeshView *mView;
    ExportMeshModel *mModel;
    AppStatus *mAppStatus;
};

} // namespace graphos

#endif // GRAPHOS_EXPORT_MESH_PRESENTER_H
