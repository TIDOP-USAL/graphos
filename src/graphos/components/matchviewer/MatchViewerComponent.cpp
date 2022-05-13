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

#include "MatchViewerComponent.h"

#include "graphos/components/matchviewer/impl/MatchViewerModel.h"
#include "graphos/components/matchviewer/impl/MatchViewerView.h"
#include "graphos/components/matchviewer/impl/MatchViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

MatchViewerComponent::MatchViewerComponent(Application *application)
  : ComponentBase(application)
{
  init();
}

MatchViewerComponent::~MatchViewerComponent()
{
}

void MatchViewerComponent::init()
{
  this->setName("Match Viewer");
  this->setMenu("tools");
  this->setToolbar("tools");

  action()->setIcon(QIcon(":/ico/24/img/material/24/view_match_24px.png"));
}

void MatchViewerComponent::openMatchesViewer(size_t imageId)
{
  action()->trigger();
  dynamic_cast<MatchViewerPresenter *>(presenter())->setLeftImage(imageId);
}

void MatchViewerComponent::createModel()
{
  setModel(new MatchViewerModelImp(app()->project()));
}

void MatchViewerComponent::createView()
{
  Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  setView(new MatchViewerViewImp(nullptr, f));
}

void MatchViewerComponent::createPresenter()
{
  setPresenter(new MatchViewerPresenterImp(dynamic_cast<MatchViewerView *>(view()), 
                                           dynamic_cast<MatchViewerModel *>(model())));
}

void MatchViewerComponent::createCommand()
{
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
