#include "CreateProjectComponent.h"

#include "graphos/ui/createproject/impl/CreateProjectModel.h"
#include "graphos/ui/createproject/impl/CreateProjectView.h"
#include "graphos/ui/createproject/impl/CreateProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


CreateProjectComponent::CreateProjectComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("New Project");
  this->setMenu("file");
  this->setToolbar("file");
  QIcon iconNewProject;
  iconNewProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-empty-document.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconNewProject);
#ifndef QT_NO_SHORTCUT
  mAction->setShortcut(tr("Ctrl+N"));
#endif // QT_NO_SHORTCUT
}

CreateProjectComponent::~CreateProjectComponent()
{
}

void CreateProjectComponent::createModel()
{
  mModel = new CreateProjectModelImp(mProject);
}

void CreateProjectComponent::createView()
{
  mView = new CreateProjectViewImp();
}

void CreateProjectComponent::createPresenter()
{
  mPresenter = new CreateProjectPresenterImp(dynamic_cast<CreateProjectView *>(mView), 
                                             dynamic_cast<CreateProjectModel *>(mModel));
  connect(dynamic_cast<CreateProjectPresenter *>(mPresenter), &CreateProjectPresenter::projectCreate, 
          this, &CreateProjectComponent::onProjectCreated);
}

void CreateProjectComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  mAction->setEnabled(!bProcessing);
}

void CreateProjectComponent::onProjectCreated()
{
  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::project_exists, true);
  
  emit projectCreated();
}

} // namespace ui

} // namespace graphos
