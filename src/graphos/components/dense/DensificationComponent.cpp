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

#include "DensificationComponent.h"

#include "graphos/components/dense/impl/DensificationModel.h"
#include "graphos/components/dense/impl/DensificationView.h"
#include "graphos/components/dense/impl/DensificationPresenter.h"
#include "graphos/components/dense/impl/DensificationCommand.h"
#include "graphos/widgets/CmvsPmvsWidget.h"
#include "graphos/widgets/SmvsWidget.h"
#include "graphos/widgets/MvsWidget.h"
#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

DensificationComponent::DensificationComponent(Application *application)
    : TaskComponent(application)
{
    init();
}

DensificationComponent::~DensificationComponent()
{
}

void DensificationComponent::enableMethod(Method method)
{
    if (method == Method::mvs && mMethod.isDisabled(method)) {
        mMethod.enable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setMvsWidget(std::make_unique<MvsWidget>());
    } else if (method == Method::pmvs && mMethod.isDisabled(method)) {
        mMethod.enable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setCmvsPmvsWidget(std::make_unique<CmvsPmvsWidgetImp>());
    } else if (method == Method::smvs && mMethod.isDisabled(method)) {
        mMethod.enable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setSmvsWidget(std::make_unique<SmvsWidgetImp>());
    }
}

void DensificationComponent::disableMethod(Method method)
{
    if (method == Method::mvs && mMethod.isEnabled(method)) {
        mMethod.disable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setMvsWidget(nullptr);
    } else if (method == Method::pmvs && mMethod.isEnabled(method)) {
        mMethod.disable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setCmvsPmvsWidget(nullptr);
    } else if (method == Method::smvs && mMethod.isEnabled(method)) {
        mMethod.disable(method);
        dynamic_cast<DensificationPresenter *>(presenter())->setSmvsWidget(nullptr);
    }
}

bool DensificationComponent::isEnabled(Method method) const
{
    return mMethod.isEnabled(method);
}

void DensificationComponent::init()
{
    setName("Densification");
    setMenu("workflow");
    setToolbar("workflow");
    setIcon(QIcon::fromTheme("dense"));

    mMethod.enable(Method::mvs);
    mMethod.enable(Method::pmvs);
    mMethod.enable(Method::smvs);

    createCommand();
}

void DensificationComponent::createModel()
{
    setModel(new DensificationModelImp(app()->project()));
}

void DensificationComponent::createView()
{
    setView(new DensificationViewImp());
}

void DensificationComponent::createPresenter()
{
    setPresenter(new DensificationPresenterImp(dynamic_cast<DensificationView *>(view()),
                 dynamic_cast<DensificationModel *>(model())));
    if (mMethod.isEnabled(Method::mvs))
        dynamic_cast<DensificationPresenter *>(presenter())->setMvsWidget(std::make_unique<MvsWidget>());
    if (mMethod.isEnabled(Method::pmvs))
        dynamic_cast<DensificationPresenter *>(presenter())->setCmvsPmvsWidget(std::make_unique<CmvsPmvsWidgetImp>());
    if (mMethod.isEnabled(Method::smvs))
        dynamic_cast<DensificationPresenter *>(presenter())->setSmvsWidget(std::make_unique<SmvsWidgetImp>());

}

void DensificationComponent::createCommand()
{
    setCommand(std::make_shared<DensificationCommand>());
}

void DensificationComponent::update()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool dense_model_active = app_status->isEnabled(AppStatus::Flag::project_exists) &&
                             !app_status->isEnabled(AppStatus::Flag::processing) &&
                              app_status->isEnabled(AppStatus::Flag::oriented);

    action()->setEnabled(dense_model_active);
}

void DensificationComponent::onRunning()
{
    TaskComponent::onRunning();
}

void DensificationComponent::onFinished()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFinished();
    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::dense_model, true);
}

void DensificationComponent::onFailed()
{
    Application *app = this->app();
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    TaskComponent::onFailed();
    app_status->activeFlag(AppStatus::Flag::dense_model, false);
}

} // namespace graphos
