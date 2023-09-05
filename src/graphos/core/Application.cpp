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

#include <QAction>
#include <QSettings>
#include <QFileInfo>
#include <QMainWindow>
#include <QStandardPaths>

namespace graphos
{

std::unique_ptr<Application> Application::sApplication;
std::mutex Application::sMutex;
std::once_flag Application::sInitFlag;

Application::Application(int &argc, char **argv)
  : QApplication(argc, argv),
    mAppStatus(new AppStatus()),
    mProject(nullptr),
    mSettings(new SettingsImp),
    mMainWindow(nullptr),
    mCommandList(nullptr),
    mViewer3D(nullptr)
{
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
}

tl::Path Application::documentsLocation() const
{
    tl::Path path(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdWString());
    path.append(qApp->applicationDisplayName().toStdWString());
    path.append("Projects");
    path.normalize();

    return path;
}

AppStatus *Application::status()
{
    return mAppStatus;
}

//tl::MessageManager *Application::messageManager()
//{
//  return &tl::MessageManager::instance();
//}

Project *Application::project()
{
    return mProject;
}

const Project *Application::project() const
{
    return mProject;
}

void Application::setProject(Project *project)
{
    mProject = project;
}

Settings *Application::settings()
{
    return mSettings;
}

QMainWindow *Application::mainWindow() const
{
    return mMainWindow;
}

void Application::setMainWindow(QMainWindow *mainWindow)
{
    mMainWindow = mainWindow;
}

Viewer3D *Application::viewer3D() const
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

tl::CommandList::Status Application::parse(int argc, char **argv)
{
    tl::CommandList::Status status = commandList()->parse(argc, argv);

    if (status == tl::CommandList::Status::parse_success)
        mAppStatus->activeFlag(AppStatus::Flag::command_mode, true);

    return status;
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

void Application::freeMemory()
{
    for (auto component : mComponents) {
        component->freeMemory();
    }
}

QStringList Application::history() const
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

Application &Application::instance()
{
    return *(static_cast<Application *>(QCoreApplication::instance()));
}

tl::CommandList *Application::commandList()
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
