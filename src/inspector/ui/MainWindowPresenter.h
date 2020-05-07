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
//class SettingsModel;
//class HelpDialog;
class TabHandler;

class MainWindowPresenter
  : public IPresenter
{
  Q_OBJECT

public:

  explicit MainWindowPresenter(MainWindowView *view,
                               MainWindowModel *model,
                               ProjectModel *projectModel/*,
                               SettingsModel *settingsModel*/);
  ~MainWindowPresenter() override;

signals:

  void openNewProjectDialog();
  void openFeatureExtractionDialog();
  void openFeatureMatchingDialog();
//  void openKeypointsViewerDialogFromSession(QString);
//  void openKeypointsViewerDialogFromSessionAndImage(QString, QString);
//  void openMatchesViewerDialogFromSession(QString);
//  void openMatchesViewerDialogFromSessionAndImages(QString, QString, QString);
//  void openExportFeaturesDialog();
//  void openExportMatchesDialog();
//  void openMultiviewMatchingAssessmentDialog();
  void openAboutDialog();
  void openSettingsDialog();
  void openViewSettingsDialog();
  void openToolSettingsDialog();

public slots:

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

  void openKeypointsViewer();
  void openMatchesViewer();

  /* Menú herramientas */

  void loadImages();

  /* Menú Ayuda */

  /*!
   * \brief Carga del proyecto
   */
  void loadProject();
  void updateProject();

  void openImage(const QString &image);
  void activeImage(const QString &image);
  void activeImages(const QStringList &images);
  void deleteImages(const QStringList &images);

  void openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2);

  void updateFeatures();
  void updateMatches();
  void deleteFeatures();
  void deleteMatches();

  void processFinish();
  void processRunning();

  void onLoadImages();

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;

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
  //SettingsModel *mSettingsModel;
  HelpDialog *mHelpDialog;
  TabHandler *mTabHandler;
  StartPageWidget *mStartPageWidget;
  QString mProjectDefaultPath;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MAINWINDOW_PRESENTER_H
