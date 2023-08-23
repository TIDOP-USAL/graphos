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

#include "ImageLoaderComponent.h"

#include "graphos/components/images/impl/ImageLoaderModel.h"
#include "graphos/components/images/impl/ImageLoaderView.h"
#include "graphos/components/images/impl/ImageLoaderPresenter.h"
#include "graphos/components/images/impl/ImageLoaderCommand.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

ImageLoaderComponent::ImageLoaderComponent(Application *application)
  : TaskComponent(application)
{
    init();
}

ImageLoaderComponent::~ImageLoaderComponent()
{
}

void ImageLoaderComponent::init()
{
    this->setName("Load Images");
    this->setMenu("workflow");
    this->setToolbar("workflow");

    createCommand();

    action()->setIcon(QIcon::fromTheme("pictures-folder"));
}

void ImageLoaderComponent::createModel()
{
    setModel(new ImageLoaderModelImp(app()->project()));
}

void ImageLoaderComponent::createView()
{
    setView(new ImageLoaderViewImp());
}

void ImageLoaderComponent::createPresenter()
{
    setPresenter(new ImageLoaderPresenterImp(dynamic_cast<ImageLoaderView *>(view()),
                 dynamic_cast<ImageLoaderModel *>(model())));

    connect(dynamic_cast<ImageLoaderPresenter *>(presenter()), &ImageLoaderPresenter::image_loaded,
            this, &ImageLoaderComponent::image_loaded);
}

void ImageLoaderComponent::createCommand()
{
    setCommand(std::make_shared<ImageLoaderCommand>());
}

void ImageLoaderComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool loading_images = app_status->isEnabled(AppStatus::Flag::loading_images);
    action()->setEnabled(project_exists && !loading_images && !processing);
}

void ImageLoaderComponent::onRunning()
{
    TaskComponent::onRunning();
}

void ImageLoaderComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();

    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::loading_images, false);
}

void ImageLoaderComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();

    app_status->activeFlag(AppStatus::Flag::loading_images, false);
}

} // namespace graphos
