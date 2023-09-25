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

#include "PropertiesPresenter.h"

#include "graphos/components/properties/PropertiesModel.h"
#include "graphos/components/properties/PropertiesView.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/exception.h>

#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{


PropertiesPresenterImp::PropertiesPresenterImp(PropertiesView *view,
                                               PropertiesModel *model,
                                               AppStatus *status)
  : PropertiesPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
    PropertiesPresenterImp::init();
    PropertiesPresenterImp::initSignalAndSlots();
}

void PropertiesPresenterImp::parseDocument(const QString &parser, const QString &file)
{
    try {

        auto properties = mModel->parse(parser, file);
        mView->setProperties(properties);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void PropertiesPresenterImp::setImageActive(size_t imageId)
{
    try {

        auto properties = mModel->exif(imageId);
        mView->setProperties(properties);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void PropertiesPresenterImp::open()
{

}

void PropertiesPresenterImp::init()
{

}

void PropertiesPresenterImp::initSignalAndSlots()
{

}

} // namespace graphos
