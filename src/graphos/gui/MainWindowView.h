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

#ifndef GRAPHOS_MAINWINDOW_VIEW_H
#define GRAPHOS_MAINWINDOW_VIEW_H

#include <unordered_map>

#include <QMainWindow>

#include <tidop/core/flags.h>

#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

namespace Ui {
class MainWindowView;
}

class QTreeWidget;
class QTreeWidgetItem;
class QComboBox;
class QGridLayout;

namespace graphos
{

class ThumbnailsWidget;
class LogWidget;
class GraphicViewer;
class TabWidget;
class ProgressBarWidget;

class MainWindowView
  : public QMainWindow
{
    Q_OBJECT

public:

    enum class Menu
    {
        file,
        file_import,
        file_export,
        view,
        workflow,
        tools,
        plugins,
        help
    };

    enum class Toolbar
    {
        file,
        view,
        workflow,
        model3d,
        tools
    };

public:

    explicit MainWindowView(QWidget *parent = nullptr);
    ~MainWindowView() override;


    /// Configuración de acciones 

    void addActionToMenu(QAction *action, Menu menuName);
    void addMenuToMenu(QMenu *menu, Menu menuName);
    void addSeparatorToMenu(Menu menu);
    void addActionToToolbar(QAction *action, Toolbar toolbar);
    void addSeparatorToToolbar(Toolbar toolbar);

    void setPropertiesWidget(QWidget *widget);

    void clear();

    /*!
     * \brief Establece el nombre del proyecto
     * \param[in] title Titulo del proyecto
     */
    void setProjectTitle(const QString &title);

    void addImage(const Image &image, const Camera &camera);
    void addImages(const std::unordered_map<size_t, Image> &images,
                   const std::map<int, Camera> &cameras);
    void setActiveImage(size_t imageId);
    void setActiveImages(const std::vector<size_t> &imageIds);
    void addFeatures(size_t imageId);
    void deleteFeatures(size_t imageId);
    void addMatches(size_t imageId);
    void deleteMatches(size_t imageId);

    // Por refactorizar ....

    void setSparseModel(const QString &sparseModel);
    void deleteSparseModel();
    void setDenseModel(const QString &sparseModel);
    void deleteDenseModel();
    void setMesh(const QString &mesh);
    void deleteMesh();
    void setDSM(const QString &dsm);
    void setDTM(const QString &dtm);
    void deleteDsm();
    void deleteDtm();
    void setOrtho(const QString &ortho);
    void deleteOrtho();

    /*!
     * \brief Añade un mensaje temporal en la barra de herramientas
     * \param msg Mensaje
     */
    void setStatusBarMsg(const QString &msg);

    ProgressBarWidget *progressBar();

    /*!
     * \brief Tab handler
     * \return
     */
    TabWidget *tabWidget();

    //QAction *actionDistanceMeasuse();

public slots:

    /*!
     * \brief Elimina una imagen
     * \param[in] file
     */
    void deleteImages(const std::vector<size_t> &imageIds);

signals:

    /* Menu File */

    void openProjectFromHistory(QString);
    void clearHistory();
    void openCamerasImport();
    //void openExportFeatures();
    //void openExportMatches();
    //void openExportOrientations();
    //void openExportPointCloud();
    //void openExportMesh();
    void exit();


    /* Menú View */

    void openStartPage();

    /* Menú herramientas */

    void openKeypointsViewer(size_t);
    void openMatchesViewer(size_t);

    /* Menú Ayuda */

    void openHelpDialog();

    void select_sparse_model();
    void select_dense_model();
    void select_mesh_model();
    void select_image(size_t);
    void select_images(std::vector<size_t>);
    void delete_images(std::vector<size_t>);
    void open_image(size_t);
    void all_tabs_closed();

    /// Sin refactorizar

    void selectFeatures(QString);
    void selectDetector(QString);
    void selectDescriptor(QString);
    void selectImageFeatures(QString);

    void open3DModel(QString, bool);
    void openDtm();
    void openDsm();
    void openOrtho(QString);
    void openMultiView(QString);

protected:

    void changeEvent(QEvent *e) override;
    void readSettings();

private slots:

    void openFromHistory();
    void onSelectionChanged();
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onTreeContextMenu(const QPoint &point);

private:

    void initUI();
    void initTabWidget();
    void initThumbnailsTool();
    void initConsole();
    void initActions();
    void initToolbars();
    void initToolbarFile();
    void initToolbarWorkflow();
    void initToolbarView();
    void initToolbar3dModel();
    void initToolbarTools();
    void initTreeWidget();
    void initMenus();
    void initMenuFile();
    void initMenuView();
    void initMenuWorkflow();
    void initMenuTools();
    void initMenuPlugins();
    void initMenuHelp();
    void initProgressBar();
    void initSignalAndSlots();

    QMenu *findMenu(Menu menu);
    QToolBar *findToolbar(MainWindowView::Toolbar toolbar);

private slots:

    void update();
    void retranslate();

// QWidget interface

public:

    void closeEvent(QCloseEvent *event) override;


private:

    Ui::MainWindowView *ui;
    QTreeWidget *mTreeWidgetProject;
    QAction *mActionNewProject;
    QAction *mActionOpenProject;
    QAction *mActionSaveProject;
    QAction *mActionSaveProjectAs;
    QAction *mActionCloseProject;
    QAction *mActionExit;
    QAction *mActionStartPage;
    QAction *mActionLoadImages;
    QAction *mActionCameras;
    QAction *mActionHelp;
    QAction *mActionImportCameras;
    QAction *mActionExportCameras;
    QAction *mActionExportTiePoints;
    QAction *mActionExportMatches;
    QAction *mActionExportPointCloud;
    QAction *mActionExportMesh;
    QAction *mActionOrtho;
    QMenu *mMenuRecentProjects;
    QMenu *mMenuImport;
    QMenu *mMenuExport;
    QMenu *mMenuPanels;
    QMenu *mMenuToolBar;
    QToolBar *mToolBarFile;
    QToolBar *mToolBarWorkflow;
    QToolBar *mToolBarTools;
    QToolBar *mToolBarView;
    QToolBar *mToolBar3dModel;

    ///TreeWidget
    QAction *mActionOpenImage;
    QAction *mActionDeleteImage;
    QAction *mActionViewKeypoints;
    QAction *mActionViewMatches;
    QAction *mActionOpenModel3D;
    QMenu *mMenuTreeProjectImages;
    QMenu *mMenuTreeProjectImage;
    QMenu *mMenuTreeProjectModel3D;

    QAction *mActionZoomIn;
    QAction *mActionZoomOut;
    QAction *mActionZoomExtend;
    QAction *mActionZoom11;
    QAction *mActionGlobalZoom;
    QAction *mActionViewFront;
    QAction *mActionViewTop;
    QAction *mActionViewLeft;
    QAction *mActionViewRight;
    QAction *mActionViewBack;
    QAction *mActionViewBottom;
    QAction *mActionPointMeasuse;
    QAction *mActionDistanceMeasuse;
    QAction *mActionAngleMeasure;

    ThumbnailsWidget *mThumbnailsWidget;
    LogWidget *mLogWidget;
    ProgressBarWidget *mProgressBar;

    std::vector<QAction *> mHistory;
    QGridLayout *mLayoutCentral;
    TabWidget *mTabWidget;

};

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_VIEW_H
