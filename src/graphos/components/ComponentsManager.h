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

#ifndef GRAPHOS_COMPONENTS_MANAGER_H
#define GRAPHOS_COMPONENTS_MANAGER_H

#include <memory>

#include <QObject>

#include <tidop/core/flags.h>

namespace graphos
{

class Component;
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;
class ProjectModel;
class FeaturesModel;
class MatchesModel;
class ProgressHandler;
class ProgressDialog;
class HelpDialog;

class ComponentsManager
  : public QObject
{
  Q_OBJECT

public:

  enum class Flags
  {
    none = (0 << 0), 
    separator_before = (1 << 0),
    separator_after = (1 << 1)
  };

public:

  explicit ComponentsManager(QObject *parent = nullptr);
  ~ComponentsManager();

  void openApp();

  MainWindowView *mainWindowView();
  MainWindowModel *mainWindowModel();
  MainWindowPresenter *mainWindowPresenter();

  void registerComponent(Component *component,
                         Flags flags = Flags::none);
  void registerMultiComponent(const QString &name,
                              const QString &menu,
                              const QString &toolbar,
                              const std::list<std::shared_ptr<Component> > &components,
                              Flags flags = Flags::none);
  void loadPlugins();

  ProjectModel *projectModel();
  FeaturesModel *featuresModel();
  MatchesModel *matchesModel();

  HelpDialog *helpDialog();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

private:
  
  void loadPlugin(QObject *plugin);

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;

  ProjectModel *mProjectModel;
  FeaturesModel *mFeaturesModel;
  MatchesModel *mMatchesModel;

  HelpDialog *mHelpDialog;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};
ALLOW_BITWISE_FLAG_OPERATIONS(ComponentsManager::Flags)

} // namespace graphos


#endif // GRAPHOS_COMPONENTS_MANAGER_H
