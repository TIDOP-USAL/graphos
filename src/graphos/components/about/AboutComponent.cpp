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

#include "AboutComponent.h"

#include "graphos/components/about/impl/AboutModel.h"
#include "graphos/components/about/impl/AboutView.h"
#include "graphos/components/about/impl/AboutPresenter.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

AboutComponent::AboutComponent(Application *application)
  : ComponentBase(application)
{
  this->setName("About Graphos");
  this->setMenu("help");
  QIcon iconAbout;
  iconAbout.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-about.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconAbout);
}

AboutComponent::~AboutComponent()
{
}

void AboutComponent::createModel()
{
  setModel(new AboutModelImp());
}

void AboutComponent::createView()
{
  setView(new AboutViewImp());
}

void AboutComponent::createPresenter()
{
  setPresenter(new AboutPresenterImp(dynamic_cast<AboutView *>(view()), 
                                     dynamic_cast<AboutModel *>(model()), 
                                     app()->status()));
}

void AboutComponent::createCommand()
{
}

void AboutComponent::update()
{

}

} // namespace graphos
