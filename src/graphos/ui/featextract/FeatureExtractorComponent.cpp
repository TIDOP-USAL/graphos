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

#include "FeatureExtractorComponent.h"

#include "graphos/ui/featextract/impl/FeatureExtractorModel.h"
#include "graphos/ui/featextract/impl/FeatureExtractorView.h"
#include "graphos/ui/featextract/impl/FeatureExtractorPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeatureExtractorComponent::FeatureExtractorComponent(Project *project,
                                                     Application *application)
  : ProcessComponent(application),
    mProject(project)
{
  this->setName("Feature Extractor");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  action()->setIcon(QIcon(":/ico/24/img/material/24/features.png"));
}

FeatureExtractorComponent::~FeatureExtractorComponent()
{
}

void FeatureExtractorComponent::createModel()
{
  setModel(new FeatureExtractorModelImp(mProject));
}

void FeatureExtractorComponent::createView()
{
  setView(new FeatureExtractorViewImp());
}

void FeatureExtractorComponent::createPresenter()
{
  setPresenter(new FeatureExtractorPresenterImp(dynamic_cast<FeatureExtractorView *>(view()),
                                                dynamic_cast<FeatureExtractorModel *>(model())));
  
  connect(dynamic_cast<FeatureExtractorPresenter *>(presenter()), &FeatureExtractorPresenter::featuresExtracted, 
          this, &FeatureExtractorComponent::featuresExtracted);
  connect(dynamic_cast<FeatureExtractorPresenter *>(presenter()), &FeatureExtractorPresenter::featuresDeleted, 
          this, &FeatureExtractorComponent::featuresDeleted);
}

void FeatureExtractorComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status->isActive(AppStatus::Flag::images_added);
  action()->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}

void FeatureExtractorComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void FeatureExtractorComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFinished();
  app_status->activeFlag(AppStatus::Flag::feature_extraction, true);
}

void FeatureExtractorComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::feature_extraction, false);
}

} // namespace graphos
