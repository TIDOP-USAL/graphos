#ifndef INSPECTOR_COMPONENTS_MANAGER_H
#define INSPECTOR_COMPONENTS_MANAGER_H

#include <QObject>

#include <tidop/core/flags.h>

namespace inspector
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
class CamerasModel;
class CamerasPresenter;
class ProgressHandler;
class ProgressDialog;
class ExportOrientationsModel;
class ExportOrientationsPresenter;
class GeoreferenceModel;
class GeoreferencePresenter;
class CamerasImportModel;
class CamerasImportPresenter;
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
  CamerasModel *camerasModel();
  CamerasPresenter *camerasPresenter();
  FeaturesModel *featuresModel();
  MatchesModel *matchesModel();

  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  NewProjectPresenter *newProjectPresenter();
  ExportOrientationsModel *exportOrientationsModel();
  ExportOrientationsPresenter *exportOrientationsPresenter();
  GeoreferenceModel *georeferenceModel();
  GeoreferencePresenter *georeferencePresenter();
  CamerasImportModel *camerasImportModel();
  CamerasImportPresenter *camerasImportPresenter();
  ExportPointCloudModel *exportPointCloudModel();
  ExportPointCloudPresenter *exportPointCloudPresenter();

  //AboutDialog *aboutDialog();
  HelpDialog *helpDialog();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

protected slots:

  void initAndOpenNewProjectDialog();
  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initAndOpenCamerasDialog();
  void initSettingsDialog();
  void initAndOpenExportOrientationsDialog();
  void initAndOpenGeoreferenceDialog();
  void initAndOpenCamerasImportDialog();
  void initAndOpenExportPointCloudDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;
  Project *mProject;
  ProjectModel *mProjectModel;
  CamerasModel *mCamerasModel;
  CamerasPresenter *mCamerasPresenter;
  FeaturesModel *mFeaturesModel;
  MatchesModel *mMatchesModel;
  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;
  SettingsPresenter *mSettingsPresenter;

  NewProjectPresenter *mNewProjectPresenter;
  ExportOrientationsModel *mExportOrientationsModel;
  ExportOrientationsPresenter *mExportOrientationsPresenter;
  GeoreferenceModel *mGeoreferenceModel;
  GeoreferencePresenter *mGeoreferencePresenter;
  CamerasImportModel *mCamerasImportModel;
  CamerasImportPresenter *mCamerasImportPresenter;
  ExportPointCloudModel *mExportPointCloudModel;
  ExportPointCloudPresenter *mExportPointCloudPresenter;
  //AboutDialog *mAboutDialog;
  HelpDialog *mHelpDialog;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};
ALLOW_BITWISE_FLAG_OPERATIONS(ComponentsManager::Flags)

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
