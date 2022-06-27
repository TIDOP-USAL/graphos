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

#ifndef GRAPHOS_CORE_APPLICATION_H
#define GRAPHOS_CORE_APPLICATION_H

#include "graphos/graphos_global.h"

#include <tidop/core/messages.h>
#include <tidop/core/console.h>

#include <QObject>

#include <memory>
#include <mutex>

namespace tl
{
class CommandList;
}


namespace graphos
{

class AppStatus;
class Component;
class Project;
class Settings;

class Application
  : public QObject
{

  Q_OBJECT

private:

  Application();

public:

  static Application &instance();
  ~Application();

  Application(const Application &) = delete;
  Application(Application &&) = delete;
  Application operator=(const Application &) = delete;
  Application operator=(Application &&) = delete;

  AppStatus *status();
  tl::MessageManager *messageManager();
  Project *project();
  Settings *settings();

  void addComponent(Component *component);
  tl::CommandList::Status parse(int argc, char **argv);
  bool runCommand();

  void freeMemory();

  QStringList history() const;
  void addToHistory(const QString &project);
  void clearHistory();

signals:

  void image_loaded(size_t);

private:

  static std::unique_ptr<Application> sApplication;
  static std::mutex sMutex;
  static std::once_flag sInitFlag;
  AppStatus *mAppStatus;
  Project *mProject;
  Settings *mSettings;
  std::list<Component *> mComponents;
  tl::CommandList *mCommandList;
  QStringList mHistory;
};

} // namespace graphos


#endif // GRAPHOS_CORE_APPLICATION_H
