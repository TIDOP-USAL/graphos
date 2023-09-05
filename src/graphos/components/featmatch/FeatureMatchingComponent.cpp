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

#include "FeatureMatchingComponent.h"

#include "graphos/components/featmatch/impl/FeatureMatchingModel.h"
#include "graphos/components/featmatch/impl/FeatureMatchingView.h"
#include "graphos/components/featmatch/impl/FeatureMatchingPresenter.h"
#include "graphos/components/featmatch/impl/FeatureMatchingCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeatureMatchingComponent::FeatureMatchingComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

FeatureMatchingComponent::~FeatureMatchingComponent()
{
}

void FeatureMatchingComponent::init()
{
    this->setName("Feature Matching");
    this->setMenu("workflow");
    this->setToolbar("workflow");

    createCommand();

    action()->setIcon(QIcon::fromTheme("matching"));
}

void FeatureMatchingComponent::createModel()
{
    setModel(new FeatureMatchingModelImp(app()->project()));
}

void FeatureMatchingComponent::createView()
{
    setView(new FeatureMatchingViewImp());
}

void FeatureMatchingComponent::createPresenter()
{
    setPresenter(new FeatureMatchingPresenterImp(dynamic_cast<FeatureMatchingView *>(view()),
                 dynamic_cast<FeatureMatchingModel *>(model())));

    connect(dynamic_cast<FeatureMatchingPresenter *>(presenter()),
            &FeatureMatchingPresenter::matches_deleted,
            this,
            &FeatureMatchingComponent::matches_deleted);
}

void FeatureMatchingComponent::createCommand()
{
    setCommand(std::make_shared<FeatureMatchingCommand>());
}

void FeatureMatchingComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool feature_matching_active = app_status->isEnabled(AppStatus::Flag::project_exists) &&
        app_status->isEnabled(AppStatus::Flag::feature_extraction) &&
        !app_status->isEnabled(AppStatus::Flag::processing);

    action()->setEnabled(feature_matching_active);
}

void FeatureMatchingComponent::onRunning()
{
    TaskComponent::onRunning();
}

void FeatureMatchingComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::feature_matching, true);
}

void FeatureMatchingComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();

    app_status->activeFlag(AppStatus::Flag::feature_matching, false);
}

} // namespace graphos
