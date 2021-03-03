#include "FeaturesViewerComponent.h"

#include "inspector/ui/featviewer/impl/FeaturesViewerModel.h"
#include "inspector/ui/featviewer/impl/FeaturesViewerView.h"
#include "inspector/ui/featviewer/impl/FeaturesViewerPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


FeaturesViewerComponent::FeaturesViewerComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Features Viewer");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconFeaturesViewer;
  iconFeaturesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_points_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconFeaturesViewer);
}

FeaturesViewerComponent::~FeaturesViewerComponent()
{
}

void FeaturesViewerComponent::createModel()
{
  mModel = new FeaturesViewerModelImp(mProject);
}

void FeaturesViewerComponent::createView()
{
  mView = new FeaturesViewerViewImp();
}

void FeaturesViewerComponent::createPresenter()
{
  mPresenter = new FeaturesViewerPresenterImp(dynamic_cast<FeaturesViewerView *>(mView), 
                                              dynamic_cast<FeaturesViewerModel *>(mModel));
}

void FeaturesViewerComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bFeatureExtraction = app_status.isActive(AppStatus::Flag::feature_extraction);
  mAction->setEnabled(bFeatureExtraction);
}



} // namespace ui

} // namespace inspector
