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

#include "FeatureExtractorComponent.h"

#ifdef GRAPHOS_GUI
#include "graphos/components/featextract/impl/FeatureExtractorModel.h"
#include "graphos/components/featextract/impl/FeatureExtractorView.h"
#include "graphos/components/featextract/impl/FeatureExtractorPresenter.h"
#endif // GRAPHOS_GUI
#include "graphos/components/featextract/impl/FeatureExtractorCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeatureExtractorComponent::FeatureExtractorComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

FeatureExtractorComponent::~FeatureExtractorComponent()
{
}

void FeatureExtractorComponent::init()
{
    setName("Feature Extractor");
    setMenu("workflow");
    setToolbar("workflow");
    setIcon(QIcon::fromTheme("features"));

    createCommand();
}

void FeatureExtractorComponent::createModel()
{
#ifdef GRAPHOS_GUI
    setModel(new FeatureExtractorModelImp(app()->project()));
#endif // GRAPHOS_GUI
}

void FeatureExtractorComponent::createView()
{
#ifdef GRAPHOS_GUI
    setView(new FeatureExtractorViewImp());
#endif // GRAPHOS_GUI
}

void FeatureExtractorComponent::createPresenter()
{
#ifdef GRAPHOS_GUI
    setPresenter(new FeatureExtractorPresenterImp(dynamic_cast<FeatureExtractorView *>(view()),
                                                  dynamic_cast<FeatureExtractorModel *>(model())));

    connect(dynamic_cast<FeatureExtractorPresenter *>(presenter()), &FeatureExtractorPresenter::features_extracted,
            this, &FeatureExtractorComponent::features_extracted);
    connect(dynamic_cast<FeatureExtractorPresenter *>(presenter()), &FeatureExtractorPresenter::features_deleted,
            this, &FeatureExtractorComponent::features_deleted);
#endif // GRAPHOS_GUI
}

void FeatureExtractorComponent::createCommand()
{
    setCommand(std::make_shared<FeatureExtractorCommand>());
}

void FeatureExtractorComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool feature_extraction_active = app_status->isEnabled(AppStatus::Flag::project_exists) &&
                                     app_status->isEnabled(AppStatus::Flag::images_added) &&
                                    !app_status->isEnabled(AppStatus::Flag::processing);

    action()->setEnabled(feature_extraction_active);
}

void FeatureExtractorComponent::onRunning()
{
    TaskComponent::onRunning();
}

void FeatureExtractorComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::feature_extraction, true);
}

void FeatureExtractorComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
    app_status->activeFlag(AppStatus::Flag::feature_extraction, false);
}

} // namespace graphos
