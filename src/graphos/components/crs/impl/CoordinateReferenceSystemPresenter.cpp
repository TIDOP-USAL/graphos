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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "CoordinateReferenceSystemPresenter.h"

#include "graphos/components/crs/impl/CoordinateReferenceSystemModel.h"
#include "graphos/components/crs/impl/CoordinateReferenceSystemView.h"
#include "graphos/core/utils.h"

#include <tidop/core/defs.h>

#include <QFileDialog>

namespace graphos
{

CoordinateReferenceSystemPresenterImp::CoordinateReferenceSystemPresenterImp(CoordinateReferenceSystemView *view,
                                                         CoordinateReferenceSystemModel *model)
  : CoordinateReferenceSystemPresenter(),
    mView(view),
    mModel(model)
{
    this->init();
    this->initSignalAndSlots();
}

CoordinateReferenceSystemPresenterImp::~CoordinateReferenceSystemPresenterImp()
{

}

void CoordinateReferenceSystemPresenterImp::open()
{
    mModel->loadSettings();

    mView->exec();
}

void CoordinateReferenceSystemPresenterImp::init()
{

}

void CoordinateReferenceSystemPresenterImp::initSignalAndSlots()
{
    connect(mView, &DialogView::help, [&]() {
            emit help("CoordinateReferenceSystem.html");
    });
}


} // namespace graphos
