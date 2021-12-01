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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "FeatureMatchingComponent.h"

#include "graphos/components/featmatch/impl/FeatureMatchingModel.h"
#include "graphos/components/featmatch/impl/FeatureMatchingView.h"
#include "graphos/components/featmatch/impl/FeatureMatchingPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeatureMatchingComponent::FeatureMatchingComponent(Project *project,
                                                   Application *application)
  : ProcessComponent(application),
    mProject(project)
{
  this->setName("Feature Matching");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  action()->setIcon(QIcon(":/ico/24/img/material/24/icons8-match_view.png"));
}

FeatureMatchingComponent::~FeatureMatchingComponent()
{
}

void FeatureMatchingComponent::createModel()
{
  setModel(new FeatureMatchingModelImp(mProject));
}

void FeatureMatchingComponent::createView()
{
  setView(new FeatureMatchingViewImp());
}

void FeatureMatchingComponent::createPresenter()
{
  setPresenter(new FeatureMatchingPresenterImp(dynamic_cast<FeatureMatchingView *>(view()), 
                                               dynamic_cast<FeatureMatchingModel *>(model())));

  connect(dynamic_cast<FeatureMatchingPresenter *>(presenter()), &FeatureMatchingPresenter::matchingFinished, 
          this, &FeatureMatchingComponent::matchingFinished);
  connect(dynamic_cast<FeatureMatchingPresenter *>(presenter()), &FeatureMatchingPresenter::matchesDeleted,
          this, &FeatureMatchingComponent::matchesDeleted);
}

void FeatureMatchingComponent::createCommand()
{
}

void FeatureMatchingComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bFeatureExtraction = app_status->isActive(AppStatus::Flag::feature_extraction);
  action()->setEnabled(bProjectExists && bFeatureExtraction && !bProcessing);
}

void FeatureMatchingComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void FeatureMatchingComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFinished();
  app_status->activeFlag(AppStatus::Flag::feature_matching, true);
}

void FeatureMatchingComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::feature_matching, false);
}

} // namespace graphos
