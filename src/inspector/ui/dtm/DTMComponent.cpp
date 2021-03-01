#include "DTMComponent.h"

#include "inspector/ui/dtm/impl/DTMModel.h"
#include "inspector/ui/dtm/impl/DTMView.h"
#include "inspector/ui/dtm/impl/DTMPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


DTMComponent::DTMComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("DTM/DSM");
  this->setMenu("tools");
}

DTMComponent::~DTMComponent()
{
}

void DTMComponent::createModel()
{
  mModel = new DtmModelImp(mProject);
}

void DTMComponent::createView()
{
  mView = new DtmViewImp();
}

void DTMComponent::createPresenter()
{
  mPresenter = new DtmPresenterImp(dynamic_cast<DtmView *>(mView), 
                                   dynamic_cast<DtmModel *>(mModel));

  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::running,
          this, &ProcessComponent::running);
  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::finished,
          this, &ProcessComponent::finished);
  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::failed,
          this, &ProcessComponent::failed);
}

void DTMComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bAbsoluteOriented = app_status.isActive(AppStatus::Flag::absolute_oriented);
  mAction->setEnabled(bProjectExists && bAbsoluteOriented && !bProcessing);
}




} // namespace ui

} // namespace inspector
