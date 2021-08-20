#include "MatchViewerComponent.h"

#include "graphos/ui/matchviewer/impl/MatchViewerModel.h"
#include "graphos/ui/matchviewer/impl/MatchViewerView.h"
#include "graphos/ui/matchviewer/impl/MatchViewerPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


MatchViewerComponent::MatchViewerComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Match Viewer");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconMatchesViewer;
  iconMatchesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_match_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconMatchesViewer);
}

MatchViewerComponent::~MatchViewerComponent()
{
}

void MatchViewerComponent::openMatchesViewer(const QString &file)
{
  mAction->trigger();
  dynamic_cast<MatchViewerPresenter *>(mPresenter)->setLeftImage(file);
}

void MatchViewerComponent::createModel()
{
  mModel = new MatchViewerModelImp(mProject);
}

void MatchViewerComponent::createView()
{
  mView = new MatchViewerViewImp();
}

void MatchViewerComponent::createPresenter()
{
  mPresenter = new MatchViewerPresenterImp(dynamic_cast<MatchViewerView *>(mView), 
                                           dynamic_cast<MatchViewerModel *>(mModel));
}

void MatchViewerComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bFeatureMatching = app_status.isActive(AppStatus::Flag::feature_matching);
  mAction->setEnabled(bFeatureMatching);
}



} // namespace ui

} // namespace graphos
