/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/


#include "Application.h"

#include "graphos/core/AppStatus.h"
#include "graphos/core/Component.h"
#include "graphos/core/command.h"
#include "graphos/core/project.h"

#include <tidop/core/console.h>

#include <QAction>

namespace graphos
{

std::unique_ptr<Application> Application::sApplication;
std::mutex Application::sMutex;

Application::Application()
  : mAppStatus(new AppStatus()),
    mProject(new ProjectImp),
    mCommandList(new tl::CommandList("Graphos", "Graphos commands"))
{
  mCommandList->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
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

void Application::addComponent(Component *component)
{
  mComponents.push_back(component);
  if (std::shared_ptr<Command> command = component->command())
    mCommandList->push_back(command);
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

Application &Application::instance()
{
  if (sApplication.get() == nullptr) {
    std::lock_guard<std::mutex> lck(Application::sMutex);
    if (sApplication.get() == nullptr) {
      sApplication.reset(new Application());
    }
  }
  return *sApplication;
}

} // namespace graphos
