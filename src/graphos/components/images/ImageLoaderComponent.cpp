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

#include "ImageLoaderComponent.h"

#include "graphos/components/images/impl/ImageLoaderModel.h"
#include "graphos/components/images/impl/ImageLoaderView.h"
#include "graphos/components/images/impl/ImageLoaderPresenter.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

ImageLoaderComponent::ImageLoaderComponent(Project *project,
                                           Application *application)
  : ProcessComponent(application),
    mProject(project)
{
  this->setName("Load Images");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  QIcon iconLoadImages;
  iconLoadImages.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-add-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
  action()->setIcon(iconLoadImages);
}

ImageLoaderComponent::~ImageLoaderComponent()
{
}

void ImageLoaderComponent::createModel()
{
  setModel(new ImageLoaderModelImp(mProject));
}

void ImageLoaderComponent::createView()
{
  setView(new ImageLoaderViewImp());
}

void ImageLoaderComponent::createPresenter()
{
  setPresenter(new ImageLoaderPresenterImp(dynamic_cast<ImageLoaderView *>(view()), 
                                           dynamic_cast<ImageLoaderModel *>(model())));

  connect(dynamic_cast<ImageLoaderPresenter *>(presenter()), &ImageLoaderPresenter::imageLoaded, 
          this, &ImageLoaderComponent::imageLoaded);
}

void ImageLoaderComponent::update()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  bool bProjectExists = app_status->isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status->isActive(AppStatus::Flag::processing);
  bool bLoadingImages = app_status->isActive(AppStatus::Flag::loading_images);
  action()->setEnabled(bProjectExists && !bLoadingImages && !bProcessing);
}

void ImageLoaderComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void ImageLoaderComponent::onFinished()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFinished();
  app_status->activeFlag(AppStatus::Flag::loading_images, false);
}

void ImageLoaderComponent::onFailed()
{
  Application *app = this->app();
  TL_ASSERT(app != nullptr, "Application is null");
  AppStatus *app_status = app->status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  ProcessComponent::onFailed();
  app_status->activeFlag(AppStatus::Flag::loading_images, false);
}

} // namespace graphos
