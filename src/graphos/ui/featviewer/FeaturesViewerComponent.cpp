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

#include "FeaturesViewerComponent.h"

#include "graphos/ui/featviewer/impl/FeaturesViewerModel.h"
#include "graphos/ui/featviewer/impl/FeaturesViewerView.h"
#include "graphos/ui/featviewer/impl/FeaturesViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

FeaturesViewerComponent::FeaturesViewerComponent(Project *project,
                                                 Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("Features Viewer");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconFeaturesViewer;
  iconFeaturesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_points_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconFeaturesViewer);
}

FeaturesViewerComponent::~FeaturesViewerComponent()
{
}

void FeaturesViewerComponent::openKeypointsViewer(const QString &file)
{
  action()->trigger();
  dynamic_cast<FeaturesViewerPresenter *>(presenter())->setImageActive(file);
}

void FeaturesViewerComponent::createModel()
{
  setModel(new FeaturesViewerModelImp(mProject));
}

void FeaturesViewerComponent::createView()
{
  setView(new FeaturesViewerViewImp());
}

void FeaturesViewerComponent::createPresenter()
{
  setPresenter(new FeaturesViewerPresenterImp(dynamic_cast<FeaturesViewerView *>(view()), 
                                              dynamic_cast<FeaturesViewerModel *>(model())));
}

void FeaturesViewerComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bFeatureExtraction = app_status->isActive(AppStatus::Flag::feature_extraction);
  action()->setEnabled(bFeatureExtraction);
}

} // namespace graphos