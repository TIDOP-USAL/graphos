#include "DTMComponent.h"

#include "graphos/ui/dtm/impl/DTMModel.h"
#include "graphos/ui/dtm/impl/DTMView.h"
#include "graphos/ui/dtm/impl/DTMPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
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
  //connect(dynamic_cast<DtmPresenterImp *>(mPresenter), &DtmPresenter::finished,
  //        this, &DTMComponent::finished);
}

void DTMComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bAbsoluteOriented = app_status.isActive(AppStatus::Flag::absolute_oriented);
  bool bDenseModel = app_status.isActive(AppStatus::Flag::dense_model);
  mAction->setEnabled(bProjectExists && bAbsoluteOriented && bDenseModel && !bProcessing);
}

void DTMComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void DTMComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::dtm, true);
}

void DTMComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::dtm, false);
}

} // namespace ui

} // namespace graphos
