/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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


#include "CoordinateReferenceSystemComponent.h"

#include "graphos/components/crs/impl/CoordinateReferenceSystemModel.h"
#include "graphos/components/crs/impl/CoordinateReferenceSystemView.h"
#include "graphos/components/crs/impl/CoordinateReferenceSystemPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

CoordinateReferenceSystemComponent::CoordinateReferenceSystemComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

CoordinateReferenceSystemComponent::~CoordinateReferenceSystemComponent()
{
}

void CoordinateReferenceSystemComponent::init()
{
    setName(tr("Coordinate Reference System"));
}

void CoordinateReferenceSystemComponent::createModel()
{
    setModel(new CoordinateReferenceSystemModelImp(app()->project()));
}

void CoordinateReferenceSystemComponent::createView()
{
    setView(new CoordinateReferenceSystemViewImp());
    connect(dynamic_cast<CoordinateReferenceSystemViewImp *>(view()), &CoordinateReferenceSystemView::crs_changed,
            this, &CoordinateReferenceSystemComponent::crs_changed);
}

void CoordinateReferenceSystemComponent::createPresenter()
{
    setPresenter(new CoordinateReferenceSystemPresenterImp(dynamic_cast<CoordinateReferenceSystemView *>(view()),
                                                           dynamic_cast<CoordinateReferenceSystemModel *>(model())));
}

void CoordinateReferenceSystemComponent::createCommand()
{
}

void CoordinateReferenceSystemComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    action()->setEnabled(true);
}

} // namespace graphos
