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

#include "MatchViewerComponent.h"

#include "graphos/ui/matchviewer/impl/MatchViewerModel.h"
#include "graphos/ui/matchviewer/impl/MatchViewerView.h"
#include "graphos/ui/matchviewer/impl/MatchViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

MatchViewerComponent::MatchViewerComponent(Project *project,
                                           Application *application)
  : ComponentBase(application),
    mProject(project)
{
  this->setName("Match Viewer");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconMatchesViewer;
  iconMatchesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_match_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconMatchesViewer);
}

MatchViewerComponent::~MatchViewerComponent()
{
}

void MatchViewerComponent::openMatchesViewer(const QString &file)
{
  action()->trigger();
  dynamic_cast<MatchViewerPresenter *>(presenter())->setLeftImage(file);
}

void MatchViewerComponent::createModel()
{
  setModel(new MatchViewerModelImp(mProject));
}

void MatchViewerComponent::createView()
{
  setView(new MatchViewerViewImp());
}

void MatchViewerComponent::createPresenter()
{
  setPresenter(new MatchViewerPresenterImp(dynamic_cast<MatchViewerView *>(view()), 
                                           dynamic_cast<MatchViewerModel *>(model())));
}

void MatchViewerComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bFeatureMatching = app_status->isActive(AppStatus::Flag::feature_matching);
  action()->setEnabled(bFeatureMatching);
}

} // namespace graphos
