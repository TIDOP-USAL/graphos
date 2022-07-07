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

namespace graphos
{

std::unique_ptr<Application> Application::sApplication;
std::mutex Application::sMutex;
std::once_flag Application::sInitFlag;

Application::Application()
  : mAppStatus(new AppStatus()),
    mProject(new ProjectImp),
    mSettings(new SettingsImp),
    mCommandList(new tl::CommandList("Graphos", "Graphos commands"))
{
  mCommandList->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));

  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos");
  QStringList history = settings.value("HISTORY/RecentProjects", mHistory).toStringList();
  mHistory.clear();
  for (auto &prj : history) {
    if (QFileInfo(prj).exists()) {
      mHistory.push_back(prj);
    }
  }
}

Application::~Application()
{
  if (mAppStatus != nullptr) {
    delete mAppStatus;
    mAppStatus = nullptr;
  }

  if (mProject) {
    delete mProject;
    mProject = nullptr;
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

  //sApplication.release();
}

AppStatus *Application::status()
{
  return mAppStatus;
}

tl::MessageManager *Application::messageManager()
{
  return &tl::MessageManager::instance();
}

Project *Application::project()
{
  return mProject;
}

Settings *Application::settings()
{
  return mSettings;
}

void Application::addComponent(Component *component)
{
  if (component) {
    mComponents.push_back(component);
    if (std::shared_ptr<Command> command = component->command())
      mCommandList->push_back(command);
  }
}

tl::CommandList::Status Application::parse(int argc, char **argv)
{
  tl::CommandList::Status status = mCommandList->parse(argc, argv);

  if (status == tl::CommandList::Status::parse_success)
    mAppStatus->activeFlag(AppStatus::Flag::command_mode, true);

  return status;
}

bool Application::runCommand()
{
  bool err = false;

  for (auto component : mComponents) {
    if (component->command() && component->command()->name() == mCommandList->commandName()) {
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
  return mHistory;
}

void Application::addToHistory(const QString &project)
{
  mHistory.removeAll(project);
  mHistory.prepend(project);

  while (mHistory.size() > mSettings->historyMaxSize())
    mHistory.removeLast();

  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos");
  settings.setValue("HISTORY/RecentProjects", mHistory);

  emit update_history();
}

void Application::clearHistory()
{
  mHistory.clear();
  emit update_history();
}

Application &Application::instance()
{
  std::call_once(sInitFlag, []() {
    sApplication.reset(new Application());
  });

  return *sApplication;
}

} // namespace graphos
