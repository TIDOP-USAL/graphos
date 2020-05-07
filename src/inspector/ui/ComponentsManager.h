#ifndef INSPECTOR_COMPONENTS_MANAGER_H
#define INSPECTOR_COMPONENTS_MANAGER_H

#include <QObject>

namespace inspector
{

class ProjectController;
class Project;
class Settings;
class SettingsController;

namespace ui
{
	
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;
class ProjectModel;
class SettingsModel;
class SettingsPresenter;
class NewProjectPresenter;


class ComponentsManager
  : public QObject
{
  Q_OBJECT

public:

  explicit ComponentsManager(QObject *parent = nullptr);
  ~ComponentsManager();


  MainWindowView *mainWindowView();
  MainWindowModel *mainWindowModel();
  MainWindowPresenter *mainWindowPresenter();

  ProjectModel *projectModel();
  
  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  NewProjectPresenter *newProjectPresenter();
  
signals:

protected slots:

  void initAndOpenNewProjectDialog();
  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initSettingsDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;
  Project *mProject;
  ProjectController *mProjectController;
  ProjectModel *mProjectModel;
  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;
  SettingsPresenter *mSettingsPresenter;

  NewProjectPresenter *mNewProjectPresenter;


};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
