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
    init();
}

void AboutPresenterImp::open()
{
    mView->exec();
}

void AboutPresenterImp::init()
{
    tl::Licence licence = mModel->graphosLicence();

    // Set Graphos licence

    licence.productName();
    licence.version();
    mView->setGraphosVersion(QString::fromStdString(licence.version()));
    mView->setGraphosLicence(mModel->readLicence(QString::fromStdString(licence.text())));

    // Set licenses 

    for (const auto &licence : *mModel) {
        std::string name = licence.productName();
        name.append("  ").append(licence.version());
        QString licence_text = mModel->readLicence(QString::fromStdString(licence.text()));
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
