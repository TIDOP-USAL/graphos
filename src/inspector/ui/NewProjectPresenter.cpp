#include "NewProjectPresenter.h"

#include "inspector/ui/ProjectModel.h"
#include "inspector/ui/NewProjectView.h"
//#include "inspector/ui/HelpDialog.h"

#include <QStandardPaths>
#include <QDir>


namespace inspector
{

namespace ui
{
	
NewProjectPresenterImp::NewProjectPresenterImp(NewProjectView *view, ProjectModel *model)
  : NewProjectPresenter(),
    mView(view),
    mProjectModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

NewProjectPresenterImp::~NewProjectPresenterImp()
{
}

/* public slots */

void NewProjectPresenterImp::help()
{
//  if (mHelp){
//    mHelp->setPage("menus.html#new_project");
//    mHelp->show();
//  }
}

// INewProjectPresenter interface

// protected slots

void NewProjectPresenterImp::saveProject()
{
  ///TODO: Hay que comprobar que el nombre y la ruta sean correctos.
  /// sin que incluyan caracteres no permitidos
  ///   QRegExp re("(^(PRN|AUX|NUL|CON|COM[1-9]|LPT[1-9]|(\\.+)$)(\\..*)?$)|(([\\x00-\\x1f\\\\?*:\";|/<>])+)|(([\\. ]+)");
  /// https://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm


  QString prj_path = mView->projectPath();
  if (mView->createProjectFolder())
    prj_path.append("/").append(mView->projectName());
  mProjectModel->setProjectName(mView->projectName());
  mProjectModel->setProjectFolder(prj_path);
  QDir dir(prj_path);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
  mProjectModel->setProjectDescription(mView->projectDescription());
  prj_path.append("/").append(mView->projectName()).append(".xml");
  mProjectModel->saveAs(prj_path);

  emit projectCreate();

  mView->clear();
}

void NewProjectPresenterImp::discartProject()
{
  mView->clear();
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
  connect(mView, SIGNAL(accepted()), this, SLOT(saveProject()));
  connect(mView, SIGNAL(rejected()), this, SLOT(discartProject()));
  connect(mView, SIGNAL(help()),     this, SLOT(help()));
}

} // namespace ui

} // namespace inspector
