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
class ImagesModel;
class ImagesPresenter;
class FeaturesModel;
class MatchesModel;
class CamerasModel;
class CamerasPresenter;
//class FeatureExtractorModel;
//class FeatureExtractorPresenter;
//class FeatureMatchingModel;
//class FeatureMatchingPresenter;
//class OrientationModel;
//class OrientationPresenter;
//class DensificationModel;
//class DensificationPresenter;
class ProgressHandler;
class ProgressDialog;
//class FeaturesViewerModel;
//class FeaturesViewerPresenter;
//class MatchViewerModel;
//class MatchViewerPresenter;
class ExportOrientationsModel;
class ExportOrientationsPresenter;
class GeoreferenceModel;
class GeoreferencePresenter;
class CamerasImportModel;
class CamerasImportPresenter;
class ExportPointCloudModel;
class ExportPointCloudPresenter;
//class DtmPresenter;
//class DtmModel;
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
  ImagesModel *imagesModel();
  ImagesPresenter *imagesPresenter();
  CamerasModel *camerasModel();
  CamerasPresenter *camerasPresenter();
  FeaturesModel *featuresModel();
  MatchesModel *matchesModel();

  SettingsModel *settingsModel();
  SettingsPresenter *settingsPresenter();

  NewProjectPresenter *newProjectPresenter();
  //FeatureExtractorModel *featureExtractorModel();
  //FeatureExtractorPresenter *featureExtractorPresenter();
  //FeatureMatchingModel *featureMatchingModel();
  //FeatureMatchingPresenter *featureMatchingPresenter();
  //OrientationModel *orientationModel();
  //OrientationPresenter *orientationPresenter();
  //DensificationModel *densificationModel();
  //DensificationPresenter *densificationPresenter();
  //FeaturesViewerModel *featuresViewerModel();
  //FeaturesViewerPresenter *featuresViewerPresenter();
  //MatchViewerModel *matchesViewerModel();
  //MatchViewerPresenter *matchesViewerPresenter();
  ExportOrientationsModel *exportOrientationsModel();
  ExportOrientationsPresenter *exportOrientationsPresenter();
  GeoreferenceModel *georeferenceModel();
  GeoreferencePresenter *georeferencePresenter();
  CamerasImportModel *camerasImportModel();
  CamerasImportPresenter *camerasImportPresenter();
  ExportPointCloudModel *exportPointCloudModel();
  ExportPointCloudPresenter *exportPointCloudPresenter();
  //DtmModel *dtmModel();
  //DtmPresenter *dtmPresenter();

  //AboutDialog *aboutDialog();
  HelpDialog *helpDialog();

  ProgressHandler *progressHandler();
  ProgressDialog *progressDialog();

signals:

protected slots:

  void initAndOpenNewProjectDialog();
  void initAndOpenLoadImagesDialog();
  //void initAndOpenFeatureExtractionDialog();
  //void initAndOpenFeatureMatchingDialog();
  //void initAndOpenOrientationDialog();
  //void initAndOpenDensificationDialog();
  void initAndOpenSettingsDialog();
  void initAndOpenViewSettingsDialog();
  void initAndOpenToolSettingsDialog();
  void initAndOpenCamerasDialog();
  //void initAndOpenKeypointsViewerDialog();
  //void initAndOpenKeypointsViewerDialogFromImage(const QString &image);
  //void initAndOpenMatchesViewerDialog();
  //void initAndOpenMatchesViewerDialogFromImages(const QString &leftImage,
  //                                              const QString &rightImage);
  void initSettingsDialog();
  void initAndOpenExportOrientationsDialog();
  void initAndOpenGeoreferenceDialog();
  void initAndOpenCamerasImportDialog();
  void initAndOpenExportPointCloudDialog();
  //void initAndOpenDtmDialog();

//private:
//
//  void initKeypointsViewer();
//  void initMatchesViewer();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;
  Project *mProject;
  ProjectModel *mProjectModel;
  ImagesModel *mImagesModel;
  ImagesPresenter *mImagesPresenter;
  CamerasModel *mCamerasModel;
  CamerasPresenter *mCamerasPresenter;
  FeaturesModel *mFeaturesModel;
  MatchesModel *mMatchesModel;
  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;
  SettingsPresenter *mSettingsPresenter;

  NewProjectPresenter *mNewProjectPresenter;
  //FeatureExtractorModel *mFeatureExtractorModel;
  //FeatureExtractorPresenter *mFeatureExtractorPresenter;
  //FeatureMatchingModel *mFeatureMatchingModel;
  //FeatureMatchingPresenter *mFeatureMatchingPresenter;
  //OrientationModel *mOrientationModel;
  //OrientationPresenter *mOrientationPresenter;
  //DensificationModel *mDensificationModel;
  //DensificationPresenter *mDensificationPresenter;
  //FeaturesViewerModel *mFeaturesViewerModel;
  //FeaturesViewerPresenter *mFeaturesViewerPresenter;
  //MatchViewerModel *mMatchesViewerModel;
  //MatchViewerPresenter *mMatchesViewerPresenter;
  ExportOrientationsModel *mExportOrientationsModel;
  ExportOrientationsPresenter *mExportOrientationsPresenter;
  GeoreferenceModel *mGeoreferenceModel;
  GeoreferencePresenter *mGeoreferencePresenter;
  CamerasImportModel *mCamerasImportModel;
  CamerasImportPresenter *mCamerasImportPresenter;
  ExportPointCloudModel *mExportPointCloudModel;
  ExportPointCloudPresenter *mExportPointCloudPresenter;
  //DtmModel *mDtmModel;
  //DtmPresenter *mDtmPresenter;
  //AboutDialog *mAboutDialog;
  HelpDialog *mHelpDialog;

  ProgressHandler *mProgressHandler;
  ProgressDialog *mProgressDialog;
};
ALLOW_BITWISE_FLAG_OPERATIONS(ComponentsManager::Flags)

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
