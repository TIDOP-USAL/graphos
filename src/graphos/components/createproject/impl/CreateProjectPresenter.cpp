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

/* Qt */
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
    init();
    initSignalAndSlots();
}

void CreateProjectPresenterImp::saveProject()
{
    ///TODO: Hay que comprobar que el nombre y la ruta sean correctos.
    /// sin que incluyan caracteres no permitidos
    ///   QRegExp re("(^(PRN|AUX|NUL|CON|COM[1-9]|LPT[1-9]|(\\.+)$)(\\..*)?$)|(([\\x00-\\x1f\\\\?*:\";|/<>])+)|(([\\. ]+)");
    /// https://www.boost.org/doc/libs/1_43_0/libs/filesystem/doc/portability_guide.htm

    mModel->clear();

    tl::Path project_folder = this->projectFolder();
    project_folder.createDirectories();
    mModel->setProjectName(mView->projectName());
    mModel->setProjectFolder(project_folder);
    mModel->setProjectDescription(mView->projectDescription());
    mModel->setDatabase(databasePath(project_folder));
    mModel->saveAs(projectPath(project_folder));

    emit project_created();

    mView->clear();
}

tl::Path CreateProjectPresenterImp::projectFolder() const
{
    tl::Path project_folder = mView->projectPath().toStdWString();
    if (mView->createProjectFolderEnable())
        project_folder.append(mView->projectName().toStdWString());
    project_folder.normalize();

    return project_folder;
}

tl::Path CreateProjectPresenterImp::projectPath(const tl::Path &projectFolder) const
{
    tl::Path project_path = projectFolder;
    project_path.append(mView->projectName().append(".xml").toStdWString());
    project_path.normalize();

    return project_path;
}

tl::Path CreateProjectPresenterImp::databasePath(const tl::Path &projectFolder) const
{
    tl::Path database_path = projectFolder;
    database_path.append(mView->projectName().append(".db").toStdWString());
    database_path.normalize();

    return database_path;
}

void CreateProjectPresenterImp::discartProject()
{
    mView->clear();
}

void CreateProjectPresenterImp::checkProjectName() const
{
    tl::Path project_path = projectPath(this->projectFolder());
    mView->setExistingProject(project_path.exists());
}

void CreateProjectPresenterImp::open()
{
    if (mAppStatus->isEnabled(AppStatus::Flag::project_modified)) {
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

    mView->setProjectPath(QString::fromStdWString(mProjectsDefaultPath.toWString()));

    mView->exec();
}

void CreateProjectPresenterImp::init()
{
    mProjectsDefaultPath = dynamic_cast<Application *>(qApp)->documentsLocation();
    mProjectsDefaultPath.createDirectories();
}

void CreateProjectPresenterImp::initSignalAndSlots()
{
    connect(mView, &CreateProjectView::project_name_changed,
            this, &CreateProjectPresenterImp::checkProjectName);

    connect(mView, &QDialog::accepted,
            this, &CreateProjectPresenterImp::saveProject);

    connect(mView, &QDialog::rejected,
            this, &CreateProjectPresenterImp::discartProject);

    connect(mView, &DialogView::help, [&]() {
        emit help("menus.html#new_project");
    });
}

} // namespace graphos
