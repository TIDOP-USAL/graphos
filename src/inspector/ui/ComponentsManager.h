#ifndef INSPECTOR_COMPONENTS_MANAGER_H
#define INSPECTOR_COMPONENTS_MANAGER_H

#include <QObject>

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
class SettingsModel;
class SettingsPresenter;
class NewProjectPresenter;
class ImagesModel;
class CamerasModel;
class CamerasPresenter;
class FeatureExtractorModel;
class FeatureExtractorPresenter;
class ProgressHandler;
class ProgressDialog;

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
  ImagesModel *imagesModel();
  CamerasModel *camerasModel();
  CamerasPresenter *camerasPresenter();

  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  NewProjectPresenter *newProjectPresenter();
  FeatureExtractorModel *featureExtractorModel();
  FeatureExtractorPresenter *featureExtractorPresenter();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

protected slots:

  void initAndOpenNewProjectDialog();
  void initAndOpenFeatureExtractionDialog();
  void initAndOpenFeatureMatchingDialog();
  void initAndOpenOrientationDialog();
  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initAndOpenCamerasDialog();
  void initSettingsDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;
  Project *mProject;
  ProjectModel *mProjectModel;
  ImagesModel *mImagesModel;
  CamerasModel *mCamerasModel;
  CamerasPresenter *mCamerasPresenter;
  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;
  SettingsPresenter *mSettingsPresenter;

  NewProjectPresenter *mNewProjectPresenter;
  FeatureExtractorModel *mFeatureExtractorModel;
  FeatureExtractorPresenter *mFeatureExtractorPresenter;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
