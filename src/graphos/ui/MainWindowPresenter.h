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

#ifndef GRAPHOS_MAINWINDOW_PRESENTER_H
#define GRAPHOS_MAINWINDOW_PRESENTER_H

#include "graphos/interfaces/mvp.h"

class QTextEdit;

namespace graphos
{

class StartPageWidget;
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
  : public Presenter
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
  //void deleteImage(const QString &imageName);
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

// Presenter interface

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


} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_PRESENTER_H
