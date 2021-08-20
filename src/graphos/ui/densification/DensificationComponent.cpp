#include "DensificationComponent.h"

#include "graphos/ui/densification/impl/DensificationModel.h"
#include "graphos/ui/densification/impl/DensificationView.h"
#include "graphos/ui/densification/impl/DensificationPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


DensificationComponent::DensificationComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Densification");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  mAction->setIcon(QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
}

DensificationComponent::~DensificationComponent()
{
}

void DensificationComponent::createModel()
{
  mModel = new DensificationModelImp(mProject);
}

void DensificationComponent::createView()
{
  mView = new DensificationViewImp();
}

void DensificationComponent::createPresenter()
{
  mPresenter = new DensificationPresenterImp(dynamic_cast<DensificationView *>(mView), 
                                             dynamic_cast<DensificationModel *>(mModel));
  connect(dynamic_cast<DensificationPresenter *>(mPresenter), &DensificationPresenter::densificationFinished, 
          this, &DensificationComponent::densificationFinished);
}

void DensificationComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bOriented = app_status.isActive(AppStatus::Flag::oriented);
  mAction->setEnabled(bProjectExists && bOriented && !bProcessing);
}

void DensificationComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void DensificationComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::dense_model, true);
}

void DensificationComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::dense_model, false);
}




} // namespace ui

} // namespace graphos
