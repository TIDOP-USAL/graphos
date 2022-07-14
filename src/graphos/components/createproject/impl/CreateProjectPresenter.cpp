/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "CreateProjectPresenter.h"

#include "graphos/components/createproject/CreateProjectModel.h"
#include "graphos/components/createproject/CreateProjectView.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/path.h>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

	
CreateProjectPresenterImp::CreateProjectPresenterImp(CreateProjectView *view,
                                                     CreateProjectModel *model,
                                                     AppStatus *status)
  : CreateProjectPresenter(),
    mView(view),
    mModel(model),
    mAppStatus(status)
{
  CreateProjectPresenterImp::init();
  CreateProjectPresenterImp::initSignalAndSlots();
}

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

  emit project_created();

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
  if (mView->createProjectFolderEnable())
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
  mView->setExistingProject(QFileInfo::exists(project_path));
}

void CreateProjectPresenterImp::open()
{
  if (mAppStatus->isActive(AppStatus::Flag::project_modified)) {
    int i_ret = QMessageBox(QMessageBox::Information,
                            tr("Save Changes"),
                            tr("There are unsaved changes. Do you want to save them?"),
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
    if (i_ret == QMessageBox::Yes) {
      saveProject();
      mAppStatus->clear();
    } else if (i_ret == QMessageBox::Cancel) {
      return;
    }
  }

  mModel->clear();

  mView->setProjectPath(mProjectsDefaultPath);

  mView->exec();
}

void CreateProjectPresenterImp::init()
{ 
  mProjectsDefaultPath = dynamic_cast<Application *>(qApp)->documentsLocation();

  QDir dir(mProjectsDefaultPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

void CreateProjectPresenterImp::initSignalAndSlots()
{
  connect(mView, &CreateProjectView::project_name_changed, this, &CreateProjectPresenterImp::checkProjectName);

  connect(mView, &QDialog::accepted,  this, &CreateProjectPresenterImp::saveProject);
  connect(mView, &QDialog::rejected,  this, &CreateProjectPresenterImp::discartProject);

  connect(mView, &DialogView::help, [&]() {
    emit help("menus.html#new_project");
  });
}

} // namespace graphos
