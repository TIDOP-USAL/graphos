#include "GeoreferenceComponent.h"

#include "graphos/ui/georeference/impl/GeoreferenceModel.h"
#include "graphos/ui/georeference/impl/GeoreferenceView.h"
#include "graphos/ui/georeference/impl/GeoreferencePresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


GeoreferenceComponent::GeoreferenceComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Georeference");
  this->setMenu("tools");
  this->setToolbar("tools");
  QIcon iconGeoreference;
  iconGeoreference.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-coordinate-system.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconGeoreference);
}

GeoreferenceComponent::~GeoreferenceComponent()
{
}

void GeoreferenceComponent::createModel()
{
  mModel = new GeoreferenceModelImp(mProject);
}

void GeoreferenceComponent::createView()
{
  Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
  mView = new GeoreferenceViewImp(nullptr, f);
}

void GeoreferenceComponent::createPresenter()
{
  mPresenter = new GeoreferencePresenterImp(dynamic_cast<GeoreferenceView *>(mView), 
                                            dynamic_cast<GeoreferenceModel *>(mModel));
  connect(dynamic_cast<GeoreferencePresenterImp *>(mPresenter), &GeoreferencePresenter::georeferenceFinished,
          this, &GeoreferenceComponent::georeferenceFinished);
}

void GeoreferenceComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bOriented = app_status.isActive(AppStatus::Flag::oriented);
  mAction->setEnabled(bProjectExists && bOriented && !bProcessing);
}

void GeoreferenceComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void GeoreferenceComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::absolute_oriented, true);
}

void GeoreferenceComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::absolute_oriented, false);
}

} // namespace ui

} // namespace graphos
