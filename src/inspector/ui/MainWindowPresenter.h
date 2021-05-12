#ifndef INSPECTOR_MAINWINDOW_PRESENTER_H
#define INSPECTOR_MAINWINDOW_PRESENTER_H

#include "inspector/interfaces/mvp.h"

class QTextEdit;

namespace inspector
{

class StartPageWidget;


namespace ui
{

class MainWindowView;
class MainWindowModel;
class ProjectModel;
class SettingsModel;
class CamerasModel;
class FeaturesModel;
class MatchesModel;

class HelpDialog;
class TabHandler;

class MainWindowPresenter
  : public IPresenter
{
  Q_OBJECT

public:

  explicit MainWindowPresenter(MainWindowView *view,
                               MainWindowModel *model,
                               ProjectModel *projectModel,
                               SettingsModel *settingsModel,
                               FeaturesModel *featuresModel,
                               MatchesModel *matchesModel);
  ~MainWindowPresenter() override;
    

signals:

  void openCreateProjectDialog();
  void openProjectDialog();
  void openExportOrientationsDialog();
  void openExportPointCloudDialog();
  void openGeoreferenceDialog();
  void openCamerasImportDialog();
  void openAboutDialog();
  void openSettingsDialog();
  void openViewSettingsDialog();
  void openToolSettingsDialog();
  void openDtmDialog();

protected slots:

  /* Menú Archivo */

  void openFromHistory(const QString &file);
  void deleteHistory();
  void saveProject();
  void saveProjectAs();
  void closeProject();
  void exit();

  /* Menú View */

  void openStartPage();

  /* Menú Ayuda */

  /*!
   * \brief Carga del proyecto
   */
  void loadProject();
  void updateProject();
  void loadFeatures(const QString &featId);
  void loadMatches();
  void loadOrientation();
  void loadDenseModel();

  void openImage(const QString &imageName);
  void activeImage(const QString &imageName);
  void activeImages(const QStringList &imageNames);
  void deleteImages(const QStringList &imageNames);
  void deleteImage(const QString &imageName);
  void openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2);

  void openModel3D(const QString &model3D, bool loadCameras);

  void deleteFeatures();
  void deleteMatches();

  void processFinished();
  void processRunning();
  void processFailed();

  void loadingImages(bool loading);
  void loadImage(const QString &image);
  void onProjectModified();

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  void initDefaultPath();
  void initStartPage();

protected:

  MainWindowView *mView;
  MainWindowModel *mModel;
  ProjectModel *mProjectModel;
  SettingsModel *mSettingsModel;
  FeaturesModel *mFeaturesModel;
  MatchesModel *mMatchesModel;

  HelpDialog *mHelpDialog;
  TabHandler *mTabHandler;
  StartPageWidget *mStartPageWidget;
  QString mProjectDefaultPath;

};



} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MAINWINDOW_PRESENTER_H
