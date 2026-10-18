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


#include "HelpPresenter.h"

#include "graphos/components/help/impl/HelpModel.h"
#include "graphos/components/help/impl/HelpView.h"

#include <tidop/core/app/Message.h>

namespace graphos
{

HelpPresenterImp::HelpPresenterImp(HelpView *view,
                                   HelpModel *model)
  : mView(view),
    mModel(model)
{
    HelpPresenterImp::init();
    HelpPresenterImp::initSignalAndSlots();
}

HelpPresenterImp::~HelpPresenterImp() = default;

void HelpPresenterImp::showHelp(const QString &help)
{
    mView->setHelpPage(help);
    mView->show();
}

void HelpPresenterImp::open()
{
    mModel->loadSettings();


    mView->navigateHome();

    /* Configure View here */

    mView->show();
}

void HelpPresenterImp::init()
{
    mView->setUrlBase(mModel->urlBase());
    mView->setHomePage(mModel->homePage());
}

void HelpPresenterImp::initSignalAndSlots()
{
    connect(mView, &HelpView::help, [&]() {
        emit help("Help.html");
            });
}

} // namespace graphos
