#include "OpenProjectComponent.h"

#include "graphos/ui/openproject/impl/OpenProjectModel.h"
#include "graphos/ui/openproject/impl/OpenProjectView.h"
#include "graphos/ui/openproject/impl/OpenProjectPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


OpenProjectComponent::OpenProjectComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Open Project");
  this->setMenu("file");
  this->setToolbar("file");
  QIcon iconOpenProject;
  iconOpenProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-open.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconOpenProject);
#ifndef QT_NO_SHORTCUT
  mAction->setShortcut(tr("Ctrl+O"));
#endif // QT_NO_SHORTCUT
}

OpenProjectComponent::~OpenProjectComponent()
{
}

void OpenProjectComponent::createModel()
{
  mModel = new OpenProjectModelImp(mProject);
}

void OpenProjectComponent::createView()
{
  mView = new OpenProjectViewImp();
}

void OpenProjectComponent::createPresenter()
{
  mPresenter = new OpenProjectPresenterImp(dynamic_cast<OpenProjectView *>(mView), 
                                           dynamic_cast<OpenProjectModel *>(mModel));
  connect(dynamic_cast<OpenProjectPresenter *>(mPresenter), &OpenProjectPresenter::projectLoaded, 
          this, &OpenProjectComponent::projectLoaded);
}

void OpenProjectComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bLoadingImages = app_status.isActive(AppStatus::Flag::loading_images);
  mAction->setEnabled(!bLoadingImages && !bProcessing);
}

} // namespace ui

} // namespace graphos
