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

#ifndef GRAPHOS_COMPONENTS_MANAGER_H
#define GRAPHOS_COMPONENTS_MANAGER_H

#include <memory>

#include <QObject>

#include <tidop/core/flags.h>

namespace graphos
{

class Project;
class Settings;
class SettingsController;
class Component;
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;
class ProjectModel;
class SettingsModel;
class SettingsPresenter;
class NewProjectPresenter;
class FeaturesModel;
class MatchesModel;
class ProgressHandler;
class ProgressDialog;
class HelpDialog;
class AboutDialog;

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

  Project *project();
  ProjectModel *projectModel();
  FeaturesModel *featuresModel();
  MatchesModel *matchesModel();

  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  //AboutDialog *aboutDialog();
  HelpDialog *helpDialog();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

private:
  
  void loadPlugin(QObject *plugin);

protected slots:

  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initSettingsDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;
  Project *mProject;
  Component *mCreateProjectComponent;
  Component *mOpenProjectComponent;
  Component *mImportCamerasComponent;
  Component *mCamerasComponent;

  ProjectModel *mProjectModel;
  FeaturesModel *mFeaturesModel;
  MatchesModel *mMatchesModel;
  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;
  SettingsPresenter *mSettingsPresenter;

  HelpDialog *mHelpDialog;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};
ALLOW_BITWISE_FLAG_OPERATIONS(ComponentsManager::Flags)

} // namespace graphos


#endif // GRAPHOS_COMPONENTS_MANAGER_H
