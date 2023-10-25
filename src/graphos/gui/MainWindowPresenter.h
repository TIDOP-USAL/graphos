/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_MAINWINDOW_PRESENTER_H
#define GRAPHOS_MAINWINDOW_PRESENTER_H

#include "graphos/core/mvp.h"

class QTextEdit;

namespace graphos
{

class StartPageWidget;
class MainWindowView;
class MainWindowModel;

class MainWindowPresenter
  : public Presenter
{
    Q_OBJECT

public:

    explicit MainWindowPresenter(MainWindowView *view,
                                 MainWindowModel *model);
    ~MainWindowPresenter() override;

public slots:

    void loadImage(size_t imageId);
    void openImage(size_t imageId);
    void activeImage(size_t imageId);
    void activeImages(const std::vector<size_t> &imageIds);
    void deleteImages(const std::vector<size_t> &imageIds);
    void open3DModel(const QString &model3D, bool loadCameras);

signals:

    void openCreateProjectDialog();
    void openProjectDialog();
    //void openExportOrientationsDialog();
    //void openExportPointCloudDialog();
    //void openExportMesh();
    void openGeoreferenceDialog();
    void openCamerasImportDialog();
    void openSettings();
    void save();
    //void saveAs(QString);

protected slots:

    /* Menú Archivo */

    void openFromHistory(const QString &file);
    void deleteHistory();
    //void saveProject();
    void exit();

    /* Menú View */

    void openStartPage();

    /* Menú Ayuda */

    /*!
     * \brief Carga del proyecto
     */
    void loadProject();
    void updateProject();
    void loadFeatures(size_t imageId);
    void updateMatches();
    void loadOrientation();
    void loadDenseModel();
    void loadMesh();
    void loadDTM();
    void loadOrtho();

    void openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2);

    void openDtm();

    void deleteFeatures();
    void deleteMatches();

    void loadingImages(bool loading);
    void onProjectModified();

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

private:

    void initStartPage();

protected:

    MainWindowView *mView;
    MainWindowModel *mModel;
    StartPageWidget *mStartPageWidget;

};


} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_PRESENTER_H
