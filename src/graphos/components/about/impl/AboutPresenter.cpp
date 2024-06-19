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

#include "AboutPresenter.h"

#include "graphos/components/about/AboutModel.h"
#include "graphos/components/about/AboutView.h"

#include <tidop/core/licence.h>

#include <QStandardPaths>
#include <QDir>

namespace graphos
{

AboutPresenterImp::AboutPresenterImp(AboutView *view,
                                     AboutModel *model)
  : AboutPresenter(),
    mView(view),
    mModel(model)
{
    AboutPresenterImp::init();
}

void AboutPresenterImp::open()
{
    mView->exec();
}

void AboutPresenterImp::init()
{
    tl::Licence licence = mModel->licence();

    // Set Graphos licence

    mView->setVersion(QString::fromStdString(licence.version()));
    mView->setLicence(mModel->readLicence(QString::fromStdString(licence.text())));

    // Set licenses 

    for (const auto &third_party_licence : *mModel) {
        std::string name = third_party_licence.productName();
        name.append("  ").append(third_party_licence.version());
        QString licence_text = mModel->readLicence(QString::fromStdString(third_party_licence.text()));
        mView->addLicence(QString::fromStdString(name), licence_text);
    }

}

void AboutPresenterImp::initSignalAndSlots()
{
    connect(mView, &DialogView::help, [&]() {
        emit help("menus.html#about");
    });
}

} // namespace graphos
