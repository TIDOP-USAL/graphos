#include "DensificationComponent.h"

#include "inspector/ui/densification/impl/DensificationModel.h"
#include "inspector/ui/densification/impl/DensificationView.h"
#include "inspector/ui/densification/impl/DensificationPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


DensificationComponent::DensificationComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Densification");
  this->setMenu("workflow");
  //this->setToolbar("workflow");
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

  //connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::running,
  //        this, &ProcessComponent::running);
  //connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::finished,
  //        this, &ProcessComponent::finished);
  //connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::failed,
  //        this, &ProcessComponent::failed);
}

void DensificationComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bOriented = app_status.isActive(AppStatus::Flag::oriented);
  mAction->setEnabled(bProjectExists && bOriented && !bProcessing);
}




} // namespace ui

} // namespace inspector
