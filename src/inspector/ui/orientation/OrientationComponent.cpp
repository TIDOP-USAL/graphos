#include "OrientationComponent.h"

#include "inspector/ui/orientation/impl/OrientationModel.h"
#include "inspector/ui/orientation/impl/OrientationView.h"
#include "inspector/ui/orientation/impl/OrientationPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


OrientationComponent::OrientationComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Orientation");
  this->setMenu("workflow");
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
}

void OrientationComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bFeatureMatching = app_status.isActive(AppStatus::Flag::feature_matching);
  mAction->setEnabled(bProjectExists && bFeatureMatching && !bProcessing);
}


} // namespace ui

} // namespace inspector
