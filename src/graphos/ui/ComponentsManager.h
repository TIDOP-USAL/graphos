#ifndef GRAPHOS_COMPONENTS_MANAGER_H
#define GRAPHOS_COMPONENTS_MANAGER_H

#include <QObject>

#include <tidop/core/flags.h>

namespace graphos
{

class Project;
class Settings;
class SettingsController;

namespace ui
{
	
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;
class ProjectModel;
class Component;

class SettingsModel;
class SettingsPresenter;
class NewProjectPresenter;
class FeaturesModel;
class MatchesModel;
class ProgressHandler;
class ProgressDialog;
//class ExportOrientationsModel;
//class ExportOrientationsPresenter;
class ExportPointCloudModel;
class ExportPointCloudPresenter;
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

  MainWindowView *mainWindowView();
  MainWindowModel *mainWindowModel();
  MainWindowPresenter *mainWindowPresenter();

  void registerComponent(Component *component,
                         Flags flags = Flags::none);

  Project *project();
  ProjectModel *projectModel();
  FeaturesModel *featuresModel();
  MatchesModel *matchesModel();

  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  //ExportOrientationsModel *exportOrientationsModel();
  //ExportOrientationsPresenter *exportOrientationsPresenter();
  ExportPointCloudModel *exportPointCloudModel();
  ExportPointCloudPresenter *exportPointCloudPresenter();

  //AboutDialog *aboutDialog();
  HelpDialog *helpDialog();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

protected slots:

  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initSettingsDialog();
  //void initAndOpenExportOrientationsDialog();
  void initAndOpenExportPointCloudDialog();

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

  //ExportOrientationsModel *mExportOrientationsModel;
  //ExportOrientationsPresenter *mExportOrientationsPresenter;
  ExportPointCloudModel *mExportPointCloudModel;
  ExportPointCloudPresenter *mExportPointCloudPresenter;
  //AboutDialog *mAboutDialog;
  HelpDialog *mHelpDialog;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};
ALLOW_BITWISE_FLAG_OPERATIONS(ComponentsManager::Flags)

} // namespace ui

} // namespace graphos


#endif // GRAPHOS_COMPONENTS_MANAGER_H
