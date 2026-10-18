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


#include "HelpComponent.h"

#include "graphos/components/help/impl/HelpModel.h"
#include "graphos/components/help/impl/HelpView.h"
#include "graphos/components/help/impl/HelpPresenter.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

HelpComponent::HelpComponent(Application *application)
  : ComponentBase(application)
{
    init();
}

HelpComponent::~HelpComponent() = default;

void HelpComponent::setHelpPage(const QString &page)
{
    this->create();
    dynamic_cast<HelpPresenter *>(presenter())->showHelp(page);
}

void HelpComponent::init()
{
    setName(tr("help"));
    setMenu("help");

    action()->setIcon(QIcon::fromTheme("help"));

#ifndef QT_NO_SHORTCUT
    action()->setShortcut(tr("F1"));
#endif // QT_NO_SHORTCUT
}

void HelpComponent::createModel()
{
    setModel(new HelpModelImp());
}

void HelpComponent::createView()
{
    setView(new HelpViewImp());
}

void HelpComponent::createPresenter()
{
    setPresenter(new HelpPresenterImp(dynamic_cast<HelpView *>(view()),
                                      dynamic_cast<HelpModel *>(model())));
}

void HelpComponent::createCommand()
{
}

void HelpComponent::update()
{
}

} // namespace graphos
