#include "FeaturesViewerComponent.h"

#include "graphos/ui/featviewer/impl/FeaturesViewerModel.h"
#include "graphos/ui/featviewer/impl/FeaturesViewerView.h"
#include "graphos/ui/featviewer/impl/FeaturesViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
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

void FeaturesViewerComponent::openKeypointsViewer(const QString &file)
{
  mAction->trigger();
  dynamic_cast<FeaturesViewerPresenter *>(mPresenter)->setImageActive(file);
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

} // namespace graphos
