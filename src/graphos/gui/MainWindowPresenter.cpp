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

#include "MainWindowPresenter.h"

#include "graphos/framework/app/Application.h"
#include "graphos/core/AppStatus.h"
#include "graphos/core/utils.h"
#include "graphos/widgets/StartPageWidget.h"
#include "graphos/widgets/TabWidget.h"
#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/ProgressBarDialog.h"
#ifdef HAVE_EASY3D
#include "graphos/widgets/viewer3d/Easy3DViewer3D.h"
#endif
#include "graphos/widgets/mapviewer/MapViewer.h"
#include "graphos/gui/MainWindowView.h"
#include "graphos/gui/MainWindowModel.h"

/* TidopLib */
#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>
#include <tidop/core/app/Logger.h>
#include <tidop/geometry/transform/Affine.h>

/* Qt */
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QSettings>

namespace graphos
{


MainWindowPresenter::MainWindowPresenter(MainWindowView *view,
                                         MainWindowModel *model)
  : Presenter(),
    mView(view),
    mModel(model),
    mStartPageWidget(nullptr)
{
    MainWindowPresenter::init();
    MainWindowPresenter::initSignalAndSlots();
}

MainWindowPresenter::~MainWindowPresenter()
{
}

void MainWindowPresenter::openProject(const QString &file)
{
    try {

        if (QFileInfo(file).exists()) {

            // Se comprueba si hay un proyecto abierto con cambios sin guardar
            Application &app = Application::instance();
            AppStatus *app_status = app.status();

            if (app_status && app_status->isEnabled(AppStatus::Flag::project_modified)) {
                int i_ret = QMessageBox(QMessageBox::Information,
                                        QApplication::translate("MainWindowPresenter", "Save Changes"),
                                        QApplication::translate("MainWindowPresenter", "There are unsaved changes. Do you want to save them?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
                if (i_ret == QMessageBox::Yes) {
                    //saveProject();
                    emit save();
                } else if (i_ret == QMessageBox::Cancel) {
                    return;
                }
            }

            mModel->clear();

            if (mModel->checkOldVersion(file.toStdWString())) {
                int i_ret = QMessageBox(QMessageBox::Information,
                                        QApplication::translate("MainWindowPresenter", "It is loading an old project"),
                                        QApplication::translate("MainWindowPresenter", "If you accept, a copy of the old project will be created"),
                                        QMessageBox::Yes | QMessageBox::No).exec();
                if (i_ret == QMessageBox::Yes) {
                    mModel->oldVersionBackup(file.toStdWString());
                    Application &app = Application::instance();
                    app.status()->activeFlag(AppStatus::Flag::project_modified, true);
                } else if (i_ret == QMessageBox::Cancel) {
                    return;
                }

            }

            mModel->load(file.toStdWString());
            loadProject();

        } else {
            QByteArray ba = file.toLocal8Bit();
            const char *cfile = ba.data();
            tl::Message::warning("Project file not found: {}", file.toStdString());
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::deleteHistory()
{
    Application::instance().clearHistory();
    mStartPageWidget->setHistory(QStringList());
}

//void MainWindowPresenter::saveProject()
//{
//  try {
//    
//    mModel->save();
//    
//    Application &app = Application::instance();
//    app.status()->activeFlag(AppStatus::Flag::project_modified, false);
// 
//  } catch (std::exception &e) {
//    tl::printException(e);
//  }
//}

void MainWindowPresenter::exit()
{
    ///TODO: Se cierra la aplicación
    /// - Comprobar que no haya ningún proceso corriendo

    Application &app = Application::instance();

    if (AppStatus *app_status = app.status()) {

        if (app_status->isEnabled(AppStatus::Flag::processing)) {
            int i_ret = QMessageBox(QMessageBox::Warning,
                                    QApplication::translate("MainWindowPresenter", "Warning"),
                                    QApplication::translate("MainWindowPresenter", "Stop the current process before closing the program."),
                                    QMessageBox::Yes).exec();
            if (i_ret == QMessageBox::Yes) {
                return;
            }
        }

        if (app_status->isEnabled(AppStatus::Flag::project_modified)) {
            int i_ret = QMessageBox(QMessageBox::Information,
                                    QApplication::translate("MainWindowPresenter", "Save Changes"),
                                    QApplication::translate("MainWindowPresenter", "There are unsaved changes. Do you want to save the changes before closing the project?"),
                                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
            if (i_ret == QMessageBox::Yes) {
                //saveProject();
                emit save();
            } else if (i_ret == QMessageBox::Cancel) {
                return;
            }
        }
    }
}

void MainWindowPresenter::openStartPage()
{
    initStartPage();

    auto tab = mView->tabWidget();

    const QSignalBlocker blocker(tab);
    int id = -1;
    for (int i = 0; i < tab->count(); i++) {
        if (tab->tabText(i) == mStartPageWidget->windowTitle()) {
            id = -1;
            tab->setCurrentIndex(id);
            break;
        }
    }

    if (id == -1) {
        id = tab->addTab(mStartPageWidget, mStartPageWidget->windowTitle());
        tab->setCurrentIndex(id);
        tab->setTabToolTip(id, mStartPageWidget->windowTitle());
        tab->setTabIcon(id, QIcon::fromTheme("start-page"));
    }

    AppStatus *status = Application::instance().status();
    status->activeFlag(AppStatus::Flag::tab_image_active, false);
    status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);

}

void MainWindowPresenter::loadProject()
{
    auto &log = tl::Logger::instance();
    tl::Path log_path = mModel->projectPath();
    log_path.replaceExtension(".log");
    log.open(log_path);

    mView->clear();

    mView->setProjectTitle(mModel->projectName());
    Application &app = Application::instance();
    app.status()->activeFlag(AppStatus::Flag::project_exists, true);

    QString project_path = QString::fromStdWString(mModel->projectPath().toWString());

    /// Se añade al historial de proyectos recientes
    app.addToHistory(project_path);
    mStartPageWidget->setHistory(app.history());

    QString msg = QApplication::translate("MainWindowPresenter", "Load project: ").append(project_path);
    mView->setStatusBarMsg(msg);

    tl::Message::info("Load project: {}", mModel->projectPath().toUtf8());

    {
        const auto &images = mModel->images();

        if (!images.empty()) {
            mView->addImages(images.all(), mModel->cameras().all());
            app.status()->activeFlag(AppStatus::Flag::images_added, true);
        }

    }

    for (const auto &feat : mModel->features()) {
        this->loadFeatures(feat.first);
    }

    this->updateMatches();
    this->loadOrientation();
    this->loadDenseModel();
    this->loadMesh();
    this->loadDTM();
    this->loadDSM();
    this->loadOrtho();
}

void MainWindowPresenter::updateProject()
{
    Application &app = Application::instance();
    AppStatus *app_status = app.status();

    this->loadOrtho();
    this->loadDTM();
    this->loadDSM();
    this->loadMesh();
    this->loadDenseModel();
    this->loadOrientation();
    this->updateMatches();
}

void MainWindowPresenter::loadFeatures(size_t imageId)
{
    mView->addFeatures(imageId);
    Application &app = Application::instance();
    if (!app.status()->isEnabled(AppStatus::Flag::feature_extraction))
        app.status()->activeFlag(AppStatus::Flag::feature_extraction, true);
}

void MainWindowPresenter::updateMatches()
{
    Application &app = Application::instance();

    app.status()->activeFlag(AppStatus::Flag::feature_matching, mModel->matches().hasMatches());
    const auto image_ids_with_pairs = mModel->imageIdsWithPairs();

    for (const auto &image : mModel->images()) {

        size_t image_left_id = image.first;
        if (image_ids_with_pairs.find(image_left_id) != image_ids_with_pairs.end()) {
            mView->addMatches(image_left_id);
        } else {
            mView->deleteMatches(image_left_id);
        }
    }
}

void MainWindowPresenter::loadOrientation()
{
    TL_TODO("completar")

    Application &app = Application::instance();
    AppStatus *app_status = app.status();

    QString sparse_model = QString::fromStdWString(mModel->sparseModel().toWString());

    if (!sparse_model.isEmpty()) {
        mView->setSparseModel(sparse_model);
        TL_TODO("Por ahora lo añado aqui aunque hay que revisarlo")

        app_status->activeFlag(AppStatus::Flag::oriented, true);
        app_status->activeFlag(AppStatus::Flag::absolute_oriented, mModel->isAbsoluteOrientation());
    } else {
        mView->deleteSparseModel();
        app_status->activeFlag(AppStatus::Flag::oriented, false);
        app_status->activeFlag(AppStatus::Flag::absolute_oriented, false);
    }
}

void MainWindowPresenter::loadDenseModel()
{
    Application &app = Application::instance();

    QString dense_model = QString::fromStdWString(mModel->denseModel().toWString());

    if (!dense_model.isEmpty()) {
        mView->setDenseModel(dense_model);

        app.status()->activeFlag(AppStatus::Flag::dense_model, true);
    } else {
        mView->deleteDenseModel();
        app.status()->activeFlag(AppStatus::Flag::dense_model, false);
    }
}

void MainWindowPresenter::loadMesh()
{
    Application &app = Application::instance();

    QString mesh = QString::fromStdWString(mModel->mesh().toWString());

    if (!mesh.isEmpty()) {
        mView->setMesh(mesh);
        app.status()->activeFlag(AppStatus::Flag::mesh, true);
    } else {
        mView->deleteMesh();
        app.status()->activeFlag(AppStatus::Flag::mesh, false);
    }
}

void MainWindowPresenter::loadDTM()
{
    Application &app = Application::instance();
    QString dtm = QString::fromStdWString(mModel->dtm().toWString());
    if (!dtm.isEmpty()) {
        mView->setDTM(dtm);
        app.status()->activeFlag(AppStatus::Flag::dtm, true);
    } else {
        mView->deleteDtm();
        app.status()->activeFlag(AppStatus::Flag::dtm, false);
    }
}

void MainWindowPresenter::loadDSM()
{
    Application &app = Application::instance();
    QString dsm = QString::fromStdWString(mModel->dsm().toWString());
    if (!dsm.isEmpty()) {
        mView->setDSM(dsm);
        app.status()->activeFlag(AppStatus::Flag::dsm, true);
    } else {
        mView->deleteDsm();
        app.status()->activeFlag(AppStatus::Flag::dsm, false);
    }
}

void MainWindowPresenter::loadOrtho()
{
    try {
        Application &app = Application::instance();
        const auto &orthos = mModel->orthophotos();

        if (!orthos.empty()) {
            for (const auto &[ortho_id, ortho_path] : orthos)
                mView->addOrtho(ortho_id, QString::fromStdString(ortho_path.toString()));
            app.status()->activeFlag(AppStatus::Flag::ortho, true);
        } else {
            mView->deleteOrthos();
            app.status()->activeFlag(AppStatus::Flag::ortho, false);
        }
    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::openImage(size_t imageId)
{
    try {

        auto tab_widget = mView->tabWidget();

        if (auto graphos_image = mModel->images().find(imageId)) {
            auto &image_path = graphos_image->path();
            QString image = QString::fromStdWString(image_path.toWString());
            int tab_id = tab_widget->fileTab(image);

            if (tab_id != -1) {
                tab_widget->setCurrentIndex(tab_id);
            } else {
                GraphicViewer *graphic_viewer = new GraphicViewer(mView);
                graphic_viewer->setBackgroundBrush(QBrush(QColor(mModel->graphicViewerBackgroundColor())));
                graphic_viewer->setImage(mModel->readImage(image_path));
                tab_id = tab_widget->addTab(graphic_viewer, QFileInfo(image).fileName());
                tab_widget->setCurrentIndex(tab_id);
                tab_widget->setTabToolTip(tab_id, image);
                tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));

                graphic_viewer->zoomExtend();
            }

            AppStatus *status = Application::instance().status();
            status->activeFlag(AppStatus::Flag::tab_image_active, true);
            status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }


}

void MainWindowPresenter::activeImage(size_t imageId)
{
    try {

        mView->setActiveImage(imageId);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::activeImages(const std::vector<size_t> &imageIds)
{
    mView->setActiveImages(imageIds);
}

void MainWindowPresenter::deleteImages(const std::vector<size_t> &imageIds)
{
    try {

        mModel->deleteImages(imageIds);
        mView->deleteImages(imageIds);

        Application &app = Application::instance();
        AppStatus *app_status = app.status();
        app_status->activeFlag(AppStatus::Flag::project_modified, true);
        app_status->activeFlag(AppStatus::Flag::images_added, mModel->images().size() != 0);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2)
{

}

void MainWindowPresenter::open3DModel(const QString &model3D,
                                      bool loadCameras)
{
#ifdef HAVE_EASY3D
    try {

        QFileInfo fileInfo(model3D);
        TL_ASSERT(fileInfo.exists(), "File not found");

        auto tab_widget = mView->tabWidget();
        int tab_id = tab_widget->fileTab(model3D);

        if (tab_id != -1) {
            tab_widget->setCurrentIndex(tab_id);
            return;
        }

        auto *viewer3D = new Easy3DViewer3D(mView);
        tab_id = tab_widget->addTab(viewer3D, fileInfo.fileName());
        tab_widget->setCurrentIndex(tab_id);
        tab_widget->setTabToolTip(tab_id, model3D);

        // Let Qt paint the new tab before the synchronous file read starts.
        QApplication::processEvents();

        auto *progress_dialog = new ProgressBarDialog(mView);
        progress_dialog->setWindowTitle(QApplication::translate("MainWindowPresenter", "Loading 3D model"));
        progress_dialog->setRange(0, 0);
        progress_dialog->setStatusText(QApplication::translate("MainWindowPresenter", "Loading point cloud..."));
        progress_dialog->setCloseAuto(true);
        progress_dialog->setInitialized();
        QApplication::processEvents();

        viewer3D->loadFromFileAsync(model3D, [=](bool success) {
            progress_dialog->setFinished();
            if (!success) {
                progress_dialog->deleteLater();
                return;
            }

            viewer3D->setPointLighting(false);
            viewer3D->setTopView();

            if (loadCameras) {
                const auto transform = mModel->transform();
                tl::Affine<double, 3> affine(transform.block(0, 0, 3, 4));
                const auto rotation = affine.rotation();
                const bool has_transform = transform != tl::Matrix<double, 4, 4>::identity();

                for (const auto &[image_id, pose] : mModel->poses()) {
                    const auto *image = mModel->images().find(image_id);
                    if (!image)
                        continue;

                    tl::Point3d position = pose.position();
                    auto camera_rotation = pose.rotationMatrix();
                    if (has_transform) {
                        position = affine.transform(position);
                        camera_rotation = camera_rotation * rotation.inverse().toMatrix();
                    }

                    viewer3D->addCamera(image->name(),
                        position.x(),
                        position.y(),
                        position.z(),
                        camera_rotation);
                }
            }

            viewer3D->setGlobalZoom();

            AppStatus *status = Application::instance().status();
            status->activeFlag(AppStatus::Flag::tab_image_active, false);
            status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, true);
            progress_dialog->deleteLater();
        });

    } catch (const std::exception &e) {
        tl::printException(e);
    }
#else
    Q_UNUSED(model3D);
    Q_UNUSED(loadCameras);
#endif
}

void MainWindowPresenter::openDtm()
{
    try {

        QString dtm = QString::fromStdWString(mModel->dtm().toWString());
        auto tab_widget = mView->tabWidget();
        int tab_id = tab_widget->fileTab("Map"/*dtm*/);

        MapViewer *map_viewer = nullptr;

        bool zoom_extend = false;

        if (tab_id != -1) {
            tab_widget->setCurrentIndex(tab_id);
            map_viewer = dynamic_cast<MapViewer *>(tab_widget->currentWidget());
        } else {
            //GraphicViewer *graphic_viewer = new GraphicViewer(mView);
            //graphic_viewer->setBackgroundBrush(QBrush(QColor(mModel->graphicViewerBackgroundColor())));
            //graphic_viewer->setImage(mModel->readImage(mModel->dtm()));
            //tab_id = tab_widget->addTab(graphic_viewer, "Map"/*QFileInfo(dtm).fileName()*/);
            //tab_widget->setCurrentIndex(tab_id);
            //tab_widget->setTabToolTip(tab_id, dtm);
            //tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));

            //graphic_viewer->zoomExtend();
            map_viewer = new MapViewer(mView);
            tab_id = tab_widget->addTab(map_viewer, "Map");
            tab_widget->setCurrentIndex(tab_id);
            tab_widget->setTabToolTip(tab_id, "Map");
            tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));
            zoom_extend = true;
        }

        map_viewer->loadGeoTiff(dtm, true);
        

        if (zoom_extend) map_viewer->zoomExtend();

        AppStatus *status = Application::instance().status();
        status->activeFlag(AppStatus::Flag::tab_image_active, true);
        status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::openDsm()
{
    try {

        QString dsm = QString::fromStdWString(mModel->dsm().toWString());
        auto tab_widget = mView->tabWidget();
        int tab_id = tab_widget->fileTab("Map");

        MapViewer *map_viewer = nullptr;

        bool zoom_extend = false;

        if (tab_id != -1) {
            tab_widget->setCurrentIndex(tab_id);
            map_viewer = dynamic_cast<MapViewer *>(tab_widget->currentWidget());
        } else {

            //GraphicViewer *graphic_viewer = new GraphicViewer(mView);
            //graphic_viewer->setBackgroundBrush(QBrush(QColor(mModel->graphicViewerBackgroundColor())));
            //graphic_viewer->setImage(mModel->readImage(mModel->dsm()));
            //tab_id = tab_widget->addTab(graphic_viewer, QFileInfo(dsm).fileName());
            //tab_widget->setCurrentIndex(tab_id);
            //tab_widget->setTabToolTip(tab_id, dsm);
            //tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));

            //graphic_viewer->zoomExtend();

            map_viewer = new MapViewer(mView);
            tab_id = tab_widget->addTab(map_viewer, "Map");
            tab_widget->setCurrentIndex(tab_id);
            tab_widget->setTabToolTip(tab_id, "Map");
            tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));
            zoom_extend = true;
        }

        map_viewer->loadGeoTiff(dsm, true);
        

        if (zoom_extend) map_viewer->zoomExtend();

        AppStatus *status = Application::instance().status();
        status->activeFlag(AppStatus::Flag::tab_image_active, true);
        status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::openOrthophoto(size_t ortho_id)
{
    try {

        auto tab_widget = mView->tabWidget();
        int tab_id = tab_widget->fileTab("Map"/*orthophotos*/);

        MapViewer *map_viewer = nullptr;

        bool zoom_extend = false;
        if (tab_id != -1) {
            tab_widget->setCurrentIndex(tab_id);
            map_viewer = dynamic_cast<MapViewer *>(tab_widget->currentWidget());
        } else {

            map_viewer = new MapViewer(mView);
            //map_viewer->loadGeoTiff(orthophotos);
            //tab_id = tab_widget->addTab(map_viewer, "Map"/*QFileInfo(orthophotos).fileName()*/);
            //tab_widget->setCurrentIndex(tab_id);
            //tab_widget->setTabToolTip(tab_id, "Map"/*dsm*/);
            //tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));
            //map_viewer->zoomExtend();
            tab_id = tab_widget->addTab(map_viewer, "Map");
            tab_widget->setCurrentIndex(tab_id);
            tab_widget->setTabToolTip(tab_id, "Map");
            tab_widget->setTabIcon(tab_id, QIcon::fromTheme("image-file"));
            zoom_extend = true;
        }

        // Añadir método para cargar directamente la orto que quiero
        const auto &orthos = mModel->orthophotos();
        QString orthophoto = QString::fromStdWString(orthos.at(ortho_id).toWString());

        map_viewer->loadGeoTiff(orthophoto);

        if (zoom_extend) map_viewer->zoomExtend();

        AppStatus *status = Application::instance().status();
        status->activeFlag(AppStatus::Flag::tab_image_active, true);
        status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::deleteFeatures()
{
    //  TL_TODO("completar")
    //
    //  mFeaturesModel->clear();
}

void MainWindowPresenter::deleteMatches()
{
    //  TL_TODO("completar")
    //  mMatchesModel->clear();
}

void MainWindowPresenter::loadingImages(bool loading)
{
    Application &app = Application::instance();
    app.status()->activeFlag(AppStatus::Flag::loading_images, loading);
}

/// Refactorizado
void MainWindowPresenter::loadImage(size_t imageId)
{
    try {

        if (auto image = mModel->images().find(imageId)) {
            if (auto camera = mModel->cameras().find(image->cameraId())) {
                mView->addImage(*image, *camera);

                Application &app = Application::instance();
                app.status()->activeFlag(AppStatus::Flag::images_added, true);
            }
        }

    } catch (std::exception &e) {
        tl::printException(e);
    }
}

void MainWindowPresenter::onProjectModified()
{
    Application &app = Application::instance();
    app.status()->activeFlag(AppStatus::Flag::project_modified, true);
}

void MainWindowPresenter::open()
{
    mView->showMaximized();
}

void MainWindowPresenter::init()
{
    openStartPage(); /// Show Start Page

    /* Projects history */
    Application &app = Application::instance();
    //mView->updateHistory(app.history());
    mStartPageWidget->setHistory(app.history());

    bool bUseGPU = cudaEnabled(10.0, 3.0);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
    settings.setValue("UseCuda", settings.value("UseCuda", bUseGPU).toBool());
}

void MainWindowPresenter::initSignalAndSlots()
{

    /* Menú Archivo */

    connect(mView, &MainWindowView::openCamerasImport, this, &MainWindowPresenter::openCamerasImportDialog);
    //connect(mView, &MainWindowView::openExportOrientations, this, &MainWindowPresenter::openExportOrientationsDialog);
    //connect(mView, &MainWindowView::openExportPointCloud, this, &MainWindowPresenter::openExportPointCloudDialog);
    //connect(mView, &MainWindowView::openExportMesh, this, &MainWindowPresenter::openExportMesh);

    connect(mView, &MainWindowView::exit, this, &MainWindowPresenter::exit);

    /* Menú View */

    connect(mView, &MainWindowView::openStartPage, this, &MainWindowPresenter::openStartPage);

    /* Menú flujo de trabajo */

    /* Menú herramientas */

    /* Menú Ayuda */

    connect(mView, &MainWindowView::openHelpDialog, [&]() {
        emit help("");
    });

    //connect(mView, &MainWindowView::openAboutDialog,    this, &MainWindowPresenter::openAboutDialog);

    /* Visor de imagenes */

    connect(mView, &MainWindowView::open_image, this, &MainWindowPresenter::openImage);
    connect(mView, &MainWindowView::select_image, this, &MainWindowPresenter::activeImage);
    connect(mView, &MainWindowView::select_images, this, &MainWindowPresenter::activeImages);
    connect(mView, &MainWindowView::delete_images, this, &MainWindowPresenter::deleteImages);



    connect(mView, SIGNAL(open3DModel(QString, bool)), this, SLOT(open3DModel(QString, bool)));

    /* Visor DTM/DSM */

    connect(mView, &MainWindowView::openDtm, this, &MainWindowPresenter::openDtm);
    connect(mView, &MainWindowView::openDsm, this, &MainWindowPresenter::openDsm);
    connect(mView, &MainWindowView::openOrtho, this, &MainWindowPresenter::openOrthophoto);

    connect(mView, &MainWindowView::all_tabs_closed, []() {
        AppStatus *status = Application::instance().status();
        status->activeFlag(AppStatus::Flag::tab_image_active, false);
        status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, false);
    });

    connect(&Application::instance(), &Application::update_history, [&]() {
        mStartPageWidget->setHistory(Application::instance().history());
    });

    connect(mView, &MainWindowView::openProject, this, &MainWindowPresenter::openProject);
}

void MainWindowPresenter::initStartPage()
{
    TL_TODO("revisar")
    if (mStartPageWidget == nullptr) {
        mStartPageWidget = new StartPageWidget(mView);

        connect(mStartPageWidget, &StartPageWidget::openNew, this, &MainWindowPresenter::openCreateProjectDialog);
        connect(mStartPageWidget, &StartPageWidget::openProject, this, &MainWindowPresenter::openProjectDialog);
        connect(mStartPageWidget, &StartPageWidget::clearHistory, this, &MainWindowPresenter::deleteHistory);
        connect(mStartPageWidget, &StartPageWidget::openProjectFromHistory, this, &MainWindowPresenter::openProject);
        connect(mStartPageWidget, &StartPageWidget::openSettings, this, &MainWindowPresenter::openSettings);
    }
}

} // namespace graphos
