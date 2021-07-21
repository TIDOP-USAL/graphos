#include "ExportOrientationsComponent.h"

#include "inspector/ui/export/orientations/impl/ExportOrientationsModel.h"
#include "inspector/ui/export/orientations/impl/ExportOrientationsView.h"
#include "inspector/ui/export/orientations/impl/ExportOrientationsPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


ExportOrientationsComponent::ExportOrientationsComponent(Project *project)
  : ComponentBase(),
    mProject(project)
{
  this->setName("Export Cameras");
  this->setMenu("file_export");
}

ExportOrientationsComponent::~ExportOrientationsComponent()
{
}

//void ExportOrientationsComponent::onImportedCameras()
//{
//  AppStatus &app_status = AppStatus::instance();
//  app_status.activeFlag(AppStatus::Flag::project_modified, true);
//}

void ExportOrientationsComponent::createModel()
{
  mModel = new ExportOrientationsModelImp(mProject);
}

void ExportOrientationsComponent::createView()
{
  mView = new ExportOrientationsViewImp();
}

void ExportOrientationsComponent::createPresenter()
{
  mPresenter = new ExportOrientationsPresenterImp(dynamic_cast<ExportOrientationsView *>(mView), 
                                                  dynamic_cast<ExportOrientationsModel *>(mModel));
  
  //connect(dynamic_cast<ExportOrientationsPresenter *>(mPresenter), &ExportOrientationsPresenter::importedCameras,
  //        this, &ExportOrientationsComponent::onImportedCameras);
}

void ExportOrientationsComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status.isActive(AppStatus::Flag::oriented) || 
                       app_status.isActive(AppStatus::Flag::absolute_oriented);
  mAction->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}


} // namespace ui

} // namespace inspector
