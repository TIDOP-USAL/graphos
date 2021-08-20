#include "FeatureMatchingComponent.h"

#include "graphos/ui/featmatch/impl/FeatureMatchingModel.h"
#include "graphos/ui/featmatch/impl/FeatureMatchingView.h"
#include "graphos/ui/featmatch/impl/FeatureMatchingPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


FeatureMatchingComponent::FeatureMatchingComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Feature Matching");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  mAction->setIcon(QIcon(":/ico/24/img/material/24/icons8-match_view"));
}

FeatureMatchingComponent::~FeatureMatchingComponent()
{
}

void FeatureMatchingComponent::createModel()
{
  mModel = new FeatureMatchingModelImp(mProject);
}

void FeatureMatchingComponent::createView()
{
  mView = new FeatureMatchingViewImp();
}

void FeatureMatchingComponent::createPresenter()
{
  mPresenter = new FeatureMatchingPresenterImp(dynamic_cast<FeatureMatchingView *>(mView), 
                                               dynamic_cast<FeatureMatchingModel *>(mModel));

  connect(dynamic_cast<FeatureMatchingPresenter *>(mPresenter), &FeatureMatchingPresenter::matchingFinished, 
          this, &FeatureMatchingComponent::matchingFinished);
  connect(dynamic_cast<FeatureMatchingPresenter *>(mPresenter), &FeatureMatchingPresenter::matchesDeleted, 
          this, &FeatureMatchingComponent::matchesDeleted);
}

void FeatureMatchingComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bFeatureExtraction = app_status.isActive(AppStatus::Flag::feature_extraction);
  mAction->setEnabled(bProjectExists && bFeatureExtraction && !bProcessing);
}

void FeatureMatchingComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void FeatureMatchingComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::feature_matching, true);
}

void FeatureMatchingComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::feature_matching, false);
}


} // namespace ui

} // namespace graphos
