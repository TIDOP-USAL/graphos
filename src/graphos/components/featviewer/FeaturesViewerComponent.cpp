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

#include "FeaturesViewerComponent.h"

#include "graphos/components/featviewer/impl/FeaturesViewerModel.h"
#include "graphos/components/featviewer/impl/FeaturesViewerView.h"
#include "graphos/components/featviewer/impl/FeaturesViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeaturesViewerComponent::FeaturesViewerComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

FeaturesViewerComponent::~FeaturesViewerComponent()
{
}

void FeaturesViewerComponent::init()
{
    setName("Features Viewer");
    setMenu("tools");
    setToolbar("tools");

    action()->setIcon(QIcon::fromTheme("features-viewer"));
}

void FeaturesViewerComponent::openKeypointsViewer(size_t imageId)
{
    action()->trigger();
    dynamic_cast<FeaturesViewerPresenter *>(presenter())->setImageActive(imageId);
}

void FeaturesViewerComponent::createModel()
{
    setModel(new FeaturesViewerModelImp(app()->project()));
}

void FeaturesViewerComponent::createView()
{
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    setView(new FeaturesViewerViewImp(nullptr, f));
}

void FeaturesViewerComponent::createPresenter()
{
    setPresenter(new FeaturesViewerPresenterImp(dynamic_cast<FeaturesViewerView *>(view()),
                 dynamic_cast<FeaturesViewerModel *>(model())));
}

void FeaturesViewerComponent::createCommand()
{
}

void FeaturesViewerComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool bFeatureExtraction = app_status->isEnabled(AppStatus::Flag::feature_extraction);
    action()->setEnabled(bFeatureExtraction);
}

} // namespace graphos
