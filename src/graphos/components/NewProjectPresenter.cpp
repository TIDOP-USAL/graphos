#include "NewProjectPresenter.h"

#include "inspector/components/ProjectModel.h"
#include "inspector/components/NewProjectView.h"
#include "inspector/components/HelpDialog.h"

#include <QStandardPaths>
#include <QDir>


namespace inspector
{

namespace ui
{
	
NewProjectPresenterImp::NewProjectPresenterImp(NewProjectView *view,
                                               ProjectModel *model)
  : NewProjectPresenter(),
    mView(view),
    mProjectModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

/* public slots */

void NewProjectPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("menus.html#new_project");
    mHelp->show();
  }
}

// INewProjectPresenter interface

// protected slots

void NewProjectPresenterImp::saveProject()
{
  ///TODO: Hay que comprobar que el nombre y la ruta sean correctos.
  /// sin que incluyan caracteres no permitidos
  ///   QRegExp re("(^(PRN|AUX|NUL|CON|COM[1-9]|LPT[1-9]|(\\.+)$)(\\..*)?$)|(([\\x00-\\x1f\\\\?*:\";|/<>])+)|(([\\. ]+)");
  /// https://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm


  QString prj_folder = this->projectFolder();
  this->createProjectFolderIfNoExist(prj_folder);
  mProjectModel->setProjectName(mView->projectName());
  mProjectModel->setProjectFolder(prj_folder);
  mProjectModel->setProjectDescription(mView->projectDescription());
  mProjectModel->setDatabase(this->databasePath(prj_folder));
  mProjectModel->saveAs(this->projectPath(prj_folder));

  emit projectCreate();

  mView->clear();
}

void NewProjectPresenterImp::createProjectFolderIfNoExist(const QString &projectFolder)
{
  QDir dir(projectFolder);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

QString NewProjectPresenterImp::projectFolder() const
{
  QString prj_path = mView->projectPath();
  if (mView->createProjectFolder())
    prj_path.append("/").append(mView->projectName());
  return prj_path;
}

QString NewProjectPresenterImp::projectPath(const QString &projectFolder) const
{
  QString prj_path = projectFolder;
  prj_path.append("/").append(mView->projectName()).append(".xml");
  return prj_path;
}

QString NewProjectPresenterImp::databasePath(const QString &projectFolder) const
{
  QString database_path = projectFolder;
  database_path.append("/").append(mView->projectName()).append(".db");
  return database_path;
}

void NewProjectPresenterImp::discartProject()
{
  mView->clear();
}

void NewProjectPresenterImp::checkProjectName() const
{
  QString project_path = this->projectPath(this->projectFolder());
  mView->setExistingProject(QFileInfo(project_path).exists());
}

// IPresenter interface

// public slots:

void NewProjectPresenterImp::open()
{
  mProjectModel->clear();

  mView->setProjectPath(mProjectsDefaultPath);

  mView->exec();
}

void NewProjectPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

// private

void NewProjectPresenterImp::init()
{
  mProjectsDefaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  mProjectsDefaultPath.append("/inspector/Projects");

  QDir dir(mProjectsDefaultPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

void NewProjectPresenterImp::initSignalAndSlots()
{
  connect(mView, &NewProjectView::projectNameChange, this, &NewProjectPresenterImp::checkProjectName);

  connect(mView, &QDialog::accepted,                 this, &NewProjectPresenterImp::saveProject);
  connect(mView, &QDialog::rejected,                 this, &NewProjectPresenterImp::discartProject);
  connect(mView, &IDialogView::help,                 this, &NewProjectPresenterImp::help);
}

} // namespace ui

} // namespace inspector
