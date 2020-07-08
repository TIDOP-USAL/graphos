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
class ImagesModel;
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
                               ImagesModel *imagesModel,
                               CamerasModel *camerasModel,
                               FeaturesModel *featuresModel,
                               MatchesModel *matchesModel);
  ~MainWindowPresenter() override;
    
signals:

  void openNewProjectDialog();
  void openFeatureExtractionDialog();
  void openFeatureMatchingDialog();
  void openOrientationDialog();
  void openDensificationDialog();
  void openCamerasDialog();
  void openKeypointsViewerDialog();
  void openKeypointsViewerDialogFromImage(QString);
  void openMatchesViewerDialog();
  void openMatchesViewerDialogFromImages(QString, QString);
//  void openExportFeaturesDialog();
//  void openExportMatchesDialog();
  void openExportOrientationsDialog();
//  void openMultiviewMatchingAssessmentDialog();
  void openAboutDialog();
  void openSettingsDialog();
  void openViewSettingsDialog();
  void openToolSettingsDialog();

protected slots:

  /* Menú Archivo */

  void openNew();
  void openProject();
  void openFromHistory(const QString &file);
  void deleteHistory();
  void saveProject();
  void saveProjectAs();
  void closeProject();
  void exit();

  /* Menú View */

  void openStartPage();

  /* Quality Control */

//  void openKeypointsViewer();
//  void openMatchesViewer();

  /* Menú herramientas */

  void loadImages();

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

  void openModel3D(const QString &model3D);

  //void updateFeatures();
  //void updateMatches();
  void deleteFeatures();
  void deleteMatches();

  void processFinish();
  void processRunning();

  void onLoadImages();
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

//  bool loadPreprocess(const QString &session);
//  bool loadFeatures(const QString &session);
//  bool loadMatches(const QString &session);



protected:

  MainWindowView *mView;
  MainWindowModel *mModel;
  ProjectModel *mProjectModel;
  SettingsModel *mSettingsModel;
  ImagesModel *mImagesModel;
  CamerasModel *mCamerasModel;
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
