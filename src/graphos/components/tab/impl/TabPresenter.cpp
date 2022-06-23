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

#include "TabPresenter.h"

#include "graphos/components/tab/TabModel.h"
#include "graphos/components/tab/TabView.h"
#include "graphos/core/AppStatus.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

	
TabPresenterImp::TabPresenterImp(TabView *view,
                                 TabModel *model,
                                 AppStatus *status)
  : TabPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
  TabPresenterImp::init();
  TabPresenterImp::initSignalAndSlots();
}

void TabPresenterImp::init()
{

}

void TabPresenterImp::initSignalAndSlots()
{

}

void TabPresenterImp::openImage(size_t imageId)
{
  if (mView->imageLoaded(imageId)) {

  } else {

  }
  auto image = mModel->image(imageId);

  //mView->setCurrentImage(image.path());
}

void TabPresenterImp::setImageActive(size_t imageId)
{
  //auto image = mModel->image(imageId);
  //mView->setCurrentImage(image.path());
}

} // namespace graphos
