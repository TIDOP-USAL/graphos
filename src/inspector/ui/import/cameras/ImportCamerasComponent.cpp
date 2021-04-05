#include "ImportCamerasComponent.h"

#include "inspector/ui/import/cameras/impl/ImportCamerasModel.h"
#include "inspector/ui/import/cameras/impl/ImportCamerasView.h"
#include "inspector/ui/import/cameras/impl/ImportCamerasPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


ImportCamerasComponent::ImportCamerasComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Import Cameras");
  this->setMenu("file");
  this->setToolbar("file");
  //mAction->setIcon(QIcon(":/ico/24/img/material/24/features.png"));
}

ImportCamerasComponent::~ImportCamerasComponent()
{
}

void ImportCamerasComponent::onImportedCameras()
{
  AppStatus& app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::project_modified, true);
}

void ImportCamerasComponent::createModel()
{
  mModel = new ImportCamerasModelImp(mProject);
}

void ImportCamerasComponent::createView()
{
  mView = new ImportCamerasViewImp();
}

void ImportCamerasComponent::createPresenter()
{
  mPresenter = new ImportCamerasPresenterImp(dynamic_cast<ImportCamerasView *>(mView), 
                                             dynamic_cast<ImportCamerasModel *>(mModel));
  
  connect(dynamic_cast<ImportCamerasPresenter *>(mPresenter), &ImportCamerasPresenter::importedCameras,
          this, &ImportCamerasComponent::onImportedCameras);
}

void ImportCamerasComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status.isActive(AppStatus::Flag::images_added);
  mAction->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}


} // namespace ui

} // namespace inspector
