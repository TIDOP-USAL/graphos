#include "OrientationComponent.h"

#include "graphos/ui/orientation/impl/OrientationModel.h"
#include "graphos/ui/orientation/impl/OrientationView.h"
#include "graphos/ui/orientation/impl/OrientationPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


OrientationComponent::OrientationComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Orientation");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  mAction->setIcon(QIcon(":/ico/24/img/material/24/icons8-coordinate-system.png"));
}

OrientationComponent::~OrientationComponent()
{
}

void OrientationComponent::createModel()
{
  mModel = new OrientationModelImp(mProject);
}

void OrientationComponent::createView()
{
  mView = new OrientationViewImp();
}

void OrientationComponent::createPresenter()
{
  mPresenter = new OrientationPresenterImp(dynamic_cast<OrientationView *>(mView), 
                                           dynamic_cast<OrientationModel *>(mModel));

  connect(dynamic_cast<OrientationPresenter *>(mPresenter), &OrientationPresenter::orientationFinished, 
          this, &OrientationComponent::orientationFinished);
  connect(dynamic_cast<OrientationPresenter *>(mPresenter), &OrientationPresenter::orientationDeleted, 
          this, &OrientationComponent::orientationDeleted);
}

void OrientationComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bFeatureMatching = app_status.isActive(AppStatus::Flag::feature_matching);
  mAction->setEnabled(bProjectExists && bFeatureMatching && !bProcessing);
}

void OrientationComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void OrientationComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::oriented, true);
}

void OrientationComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::oriented, false);
}


} // namespace ui

} // namespace graphos
