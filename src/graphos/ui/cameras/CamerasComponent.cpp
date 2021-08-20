#include "CamerasComponent.h"

#include "graphos/ui/cameras/impl/CamerasModel.h"
#include "graphos/ui/cameras/impl/CamerasView.h"
#include "graphos/ui/cameras/impl/CamerasPresenter.h"
#include "graphos/core/project.h"
#include "graphos/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace graphos
{

namespace ui
{


CamerasComponent::CamerasComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Cameras");
  this->setMenu("tools");
  QIcon icon;
  icon.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-camera-outline-2.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(icon);
}

CamerasComponent::~CamerasComponent()
{
}

void CamerasComponent::onUpdateCameras()
{
  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::project_modified, true);
}

void CamerasComponent::createModel()
{
  mModel = new CamerasModelImp(mProject);
}

void CamerasComponent::createView()
{
  mView = new CamerasViewImp();
}

void CamerasComponent::createPresenter()
{
  mPresenter = new CamerasPresenterImp(dynamic_cast<CamerasView *>(mView), 
                                       dynamic_cast<CamerasModel *>(mModel));
  connect(dynamic_cast<CamerasPresenter *>(mPresenter), &CamerasPresenter::updateCameras,
          this, &CamerasComponent::onUpdateCameras); 
}

void CamerasComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bImagesLoaded = app_status.isActive(AppStatus::Flag::images_added);
  mAction->setEnabled(bProjectExists && bImagesLoaded);
}




} // namespace ui

} // namespace graphos
