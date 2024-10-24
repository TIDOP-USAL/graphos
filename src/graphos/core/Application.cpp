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

#include "graphos/core/Application.h"

#include "graphos/core/AppStatus.h"
#include "graphos/core/Component.h"
#include "graphos/core/command.h"
#include "graphos/core/project.h"
#include "graphos/core/settings.h"

#include <tidop/core/console.h>

#include <gdal.h>
#include <cpl_conv.h>
#include <ogr_srs_api.h>

#include <QAction>
#include <QSettings>
#include <QFileInfo>
#include <QMainWindow>
#include <QStandardPaths>

namespace graphos
{

Application::Application(int &argc, char **argv)
  : QApplication(argc, argv),
    mAppStatus(new AppStatus()),
    mProject(nullptr),
    mSettings(new SettingsImp),
    mMainWindow(nullptr),
    mCommandList(nullptr),
    mViewer3D(nullptr)
{

    tl::Path app_path(argv[0]);

#ifdef TL_OS_WINDOWS
    tl::Path graphos_path = app_path.parentPath().parentPath();
    tl::Path gdal_data_path(graphos_path);
    gdal_data_path.append("gdal\\data");
    tl::Path proj_data_path(graphos_path);
    proj_data_path.append("proj");
    CPLSetConfigOption("GDAL_DATA", gdal_data_path.toString().c_str());
#   if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3,7,0)
    CPLSetConfigOption("PROJ_DATA", proj_data_path.toString().c_str());
#   else
    std::string s_proj = proj_data_path.toString();
    const char *proj_data[]{s_proj.c_str(), nullptr};
    OSRSetPROJSearchPaths(proj_data);
#   endif
#endif // TL_OS_WINDOWS
}

Application::~Application()
{
    if (mAppStatus != nullptr) {
        delete mAppStatus;
        mAppStatus = nullptr;
    }

    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }

    if (mCommandList) {
        mCommandList->clear();
        delete mCommandList;
        mCommandList = nullptr;
    }

    GDALDestroy();
}

auto Application::documentsLocation() -> tl::Path
{
    tl::Path path(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdWString());
    path.append(qApp->applicationDisplayName().toStdWString());
    path.append("Projects");
    path.normalize();

    return path;
}

auto Application::status() const -> AppStatus*
{
    return mAppStatus;
}

auto Application::project() -> Project*
{
    return mProject;
}

auto Application::project() const -> const Project*
{
    return mProject;
}

void Application::setProject(Project *project)
{
    mProject = project;
}

auto Application::settings() const -> Settings*
{
    return mSettings;
}

auto Application::mainWindow() const -> QMainWindow*
{
    return mMainWindow;
}

void Application::setMainWindow(QMainWindow *mainWindow)
{
    mMainWindow = mainWindow;
}

auto Application::viewer3D() const -> Viewer3D*
{
    return mViewer3D;
}

void Application::setViewer3D(Viewer3D *viewer3D)
{
    mViewer3D = viewer3D;
}

void Application::addComponent(Component *component)
{
    if (component) {
        mComponents.push_back(component);
        if (std::shared_ptr<Command> command = component->command())
            commandList()->push_back(command);
    }
}

auto Application::parse(int argc, char** argv) -> bool
{
    if (commandList()->parse(argc, argv) == tl::Command::Status::parse_success) {
        mAppStatus->activeFlag(AppStatus::Flag::command_mode, true);
        return true;
    }

    return false;
}

bool Application::runCommand()
{
    bool err = false;

    for (auto component : mComponents) {
        if (component->command() && component->command()->name() == commandList()->commandName()) {
            component->command()->run();
            break;
        }
    }

    return err;
}

void Application::freeMemory() const
{
    for (auto component : mComponents) {
        component->freeMemory();
    }
}

auto Application::history() const -> QStringList
{
    if (mHistory.isEmpty()) {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope, organizationName(), applicationName());
        QStringList history = settings.value("HISTORY/RecentProjects", mHistory).toStringList();

        for (auto &prj : history) {
            if (QFileInfo(prj).exists()) {
                mHistory.push_back(prj);
            }
        }
    }

    return mHistory;
}

void Application::addToHistory(const QString &project)
{
    tl::Path path(project.toStdString());
    path.normalize();
    QString normalize_path = QString::fromStdString(path.toString());
    mHistory.removeAll(normalize_path);
    mHistory.prepend(normalize_path);

    while (mHistory.size() > mSettings->historyMaxSize())
        mHistory.removeLast();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, organizationName(), applicationName());
    settings.setValue("HISTORY/RecentProjects", mHistory);

    emit update_history();
}

void Application::clearHistory()
{
    mHistory.clear();
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, organizationName(), applicationName());
    settings.setValue("HISTORY/RecentProjects", mHistory);
    emit update_history();
}

auto Application::instance() -> Application&
{
    return *(dynamic_cast<Application *>(QCoreApplication::instance()));
}

auto Application::commandList() -> tl::CommandList*
{
    if (mCommandList == nullptr) {
        std::string command_name = applicationName().toStdString();
        std::string command_description = command_name + " commands";
        mCommandList = new tl::CommandList(command_name, command_description);
        mCommandList->setVersion(applicationVersion().toStdString());
    }

    return mCommandList;
}

} // namespace graphos
