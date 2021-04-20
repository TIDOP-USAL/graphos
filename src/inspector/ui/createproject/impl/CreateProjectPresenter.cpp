#include "CreateProjectPresenter.h"

#include "inspector/ui/createproject/CreateProjectModel.h"
#include "inspector/ui/createproject/CreateProjectView.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/AppStatus.h"

#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace inspector
{

namespace ui
{
	
CreateProjectPresenterImp::CreateProjectPresenterImp(CreateProjectView *view,
                                                     CreateProjectModel *model)
  : CreateProjectPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

/* public slots */

void CreateProjectPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("menus.html#new_project");
    mHelp->show();
  }
}

// INewProjectPresenter interface

// protected slots

void CreateProjectPresenterImp::saveProject()
{
  ///TODO: Hay que comprobar que el nombre y la ruta sean correctos.
  /// sin que incluyan caracteres no permitidos
  ///   QRegExp re("(^(PRN|AUX|NUL|CON|COM[1-9]|LPT[1-9]|(\\.+)$)(\\..*)?$)|(([\\x00-\\x1f\\\\?*:\";|/<>])+)|(([\\. ]+)");
  /// https://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm


  QString prj_folder = this->projectFolder();
  this->createProjectFolderIfNoExist(prj_folder);
  mModel->setProjectName(mView->projectName());
  mModel->setProjectFolder(prj_folder);
  mModel->setProjectDescription(mView->projectDescription());
  mModel->setDatabase(this->databasePath(prj_folder));
  mModel->saveAs(this->projectPath(prj_folder));

  emit projectCreate();

  mView->clear();
}

void CreateProjectPresenterImp::createProjectFolderIfNoExist(const QString &projectFolder)
{
  QDir dir(projectFolder);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

QString CreateProjectPresenterImp::projectFolder() const
{
  QString prj_path = mView->projectPath();
  if (mView->createProjectFolder())
    prj_path.append("/").append(mView->projectName());
  return prj_path;
}

QString CreateProjectPresenterImp::projectPath(const QString &projectFolder) const
{
  QString prj_path = projectFolder;
  prj_path.append("/").append(mView->projectName()).append(".xml");
  return prj_path;
}

QString CreateProjectPresenterImp::databasePath(const QString &projectFolder) const
{
  QString database_path = projectFolder;
  database_path.append("/").append(mView->projectName()).append(".db");
  return database_path;
}

void CreateProjectPresenterImp::discartProject()
{
  mView->clear();
}

void CreateProjectPresenterImp::checkProjectName() const
{
  QString project_path = this->projectPath(this->projectFolder());
  mView->setExistingProject(QFileInfo(project_path).exists());
}

// IPresenter interface

// public slots:

void CreateProjectPresenterImp::open()
{
  AppStatus &app_status = AppStatus::instance();
  if (app_status.isActive(AppStatus::Flag::project_modified)) {
    int i_ret = QMessageBox(QMessageBox::Information,
                            tr("Save Changes"),
                            tr("There are unsaved changes. Do you want to save them?"),
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
    if (i_ret == QMessageBox::Yes) {
      saveProject();
      app_status.clear();
    } else if (i_ret == QMessageBox::Cancel) {
      return;
    }
  }

  mModel->clear();

  mView->setProjectPath(mProjectsDefaultPath);

  mView->exec();
}

void CreateProjectPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

// private

void CreateProjectPresenterImp::init()
{
  mProjectsDefaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  mProjectsDefaultPath.append("/inspector/Projects");

  QDir dir(mProjectsDefaultPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

void CreateProjectPresenterImp::initSignalAndSlots()
{
  connect(mView, &CreateProjectView::projectNameChange, this, &CreateProjectPresenterImp::checkProjectName);

  connect(mView, &QDialog::accepted,                 this, &CreateProjectPresenterImp::saveProject);
  connect(mView, &QDialog::rejected,                 this, &CreateProjectPresenterImp::discartProject);
  connect(mView, &IDialogView::help,                 this, &CreateProjectPresenterImp::help);
}

} // namespace ui

} // namespace inspector
