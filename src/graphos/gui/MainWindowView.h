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

#include <QMainWindow>

#include <tidop/core/flags.h>

namespace Ui {
class MainWindowView;
}

class QTreeWidget;
class QTreeWidgetItem;
class QComboBox;
class QProgressBar;
class QGridLayout;

namespace graphos
{

class ThumbnailsWidget;
class LogWidget;
class GraphicViewer;
class TabWidget;

class MainWindowView
  : public QMainWindow
{
  Q_OBJECT

public:

  enum class Menu
  {
    file,
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

  enum class Flag
  {
    project_exists        = (1 << 0),  // Existe un proyecto
    project_modified      = (1 << 1),  // Se ha modificado el proyecto
    images_added          = (1 << 2),  // Se han añadido fotogramas
    image_open            = (1 << 3),  // Hay una imagen abierta
    feature_extraction    = (1 << 4),
    feature_matching      = (1 << 5),
    oriented              = (1 << 6),
    absolute_oriented     = (1 << 7),
    dense_model           = (1 << 8),
    processing            = (1 << 20),
    loading_images        = (1 << 21),
    ground_truth          = (1 << 30),
  };

public:

  explicit MainWindowView(QWidget *parent = nullptr);
  ~MainWindowView() override;


  /// Configuración de acciones 

  void setCreateProjectAction(QAction *action);
  void setOpenProjectAction(QAction *action);
  void setImportCamerasAction(QAction* action);
  void setCamerasToolAction(QAction* action);

  void addActionToMenu(QAction *action, Menu menuName);
  void addMenuToMenu(QMenu *menu, Menu menuName);
  void addSeparatorToMenu(Menu menu);
  void addActionToToolbar(QAction *action, Toolbar toolbar);
  void addSeparatorToToolbar(Toolbar toolbar);

  void clear();

  /*!
   * \brief Establece el nombre del proyecto
   * \param[in] title Titulo del proyecto
   */
  void setProjectTitle(const QString &title);

  void setFlag(Flag flag, bool value);
  void addImage(const QString &image, size_t imageId);
  void setActiveImage(size_t imageId);
  void setActiveImages(const std::vector<size_t> &imageIds);
  void addFeatures(size_t imageId);
  void deleteFeatures(size_t imageId);
  void addMatches(size_t imageId);
  void deleteMatches(size_t imageId);

  // Por refactorizar ....
  //void addImages(const QStringList &images);
  
  
  void setSparseModel(const QString &sparseModel);
  void deleteSparseModel();
  void setDenseModel(const QString &sparseModel);
  void deleteDenseModel();
  void setDSM(const QString &dsm);
  void deleteDsm();
  void setOrtho(const QString &ortho);
  void deleteOrtho();

  /*!
   * \brief Añade un mensaje temporal en la barra de herramientas
   * \param msg Mensaje
   */
  void setStatusBarMsg(const QString &msg);

  /*!
   * \brief Establece las propiedades del elemento seleccionado en el árbol de proyecto
   */
  void setProperties(const std::list<std::pair<QString, QString> > &properties);

  QProgressBar *progressBar();

  /*!
   * \brief Tab handler
   * \return
   */
  TabWidget *tabWidget();

public slots:

  /*!
   * \brief Actualiza el historial de proyectos recientes
   * \param[in] history Listado con las rutas de los últimos proyectos
   */
  void updateHistory(const QStringList &history);

  /*!
   * \brief Borra el historial de proyectos recientes
   */
  void deleteHistory();

  /*!
   * \brief Elimina una imagen
   * \param[in] file
   */
  void deleteImages(const std::vector<size_t> &imageIds);

signals:

  /* Menu File */

  //void openNew();
  //void openProject();
  void openProjectFromHistory(QString);
  void clearHistory();
  void saveProject();
  void saveProjectAs();
  void openCamerasImport();
  void openExportFeatures();
  void openExportMatches();
  void openExportOrientations();
  void openExportPointCloud();
  void closeProject();
  void exit();


  /* Menú View */

  void openStartPage();

  /* Menú flujo de trabajo */

  //void loadImages();
  //void openFeatureExtraction();
  //void openFeatureMatching();
  //void openOrientation();
  //void openDensification();

  /* Menú herramientas */

  void openCamerasDialog();
  //void openGeoreferenceDialog();
  void openKeypointsViewer(size_t);
  //void openKeypointsViewerFromImage(QString);
  void openMatchesViewer(size_t);
  //void openMatchesViewerFromImages(QString,QString);
  //void openMultiviewMatchingAssessment();
  //void openSettings();
  void openDtmDialog();

  /* Menú Ayuda */

  void openHelpDialog();
  void openAboutDialog();
  
  void selectImage(size_t);
  void selectImages(std::vector<size_t>);
  void delete_images(std::vector<size_t>);
  void openImage(size_t);
  void allTabsClosed();
  /// Sin refactorizar
  
  
  
  void selectFeatures(QString);
  void selectDetector(QString);
  void selectDescriptor(QString);
  void selectImageFeatures(QString);
  void imagesLoaded();

  void open3DModel(QString, bool);

  void openDtm();
  void openOrtho(QString);

  //void openImageMatches(QString, QString, QString);


//  void openMatchesViewer(QString);
//  void openMatchesViewer(QString, QString);
  void openMultiView(QString);

protected:

  void changeEvent(QEvent *e) override;

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


protected:

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
  QAction *mActionExportTiePoints;
  QAction *mActionExportMatches;
  QAction *mActionExportPointCloud;
  QAction *mActionOrtho;
  QAction *mActionNotRecentProjects;
  QAction *mActionClearHistory;  
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

  ThumbnailsWidget *mThumbnailsWidget;
  LogWidget *mLogWidget;
  QProgressBar *mProgressBar;

  tl::EnumFlags<Flag> mFlags;
  std::vector<QAction*> mHistory;
  QGridLayout *mLayoutCentral;
  TabWidget *mTabWidget;

private:

  Ui::MainWindowView *ui;

};

ALLOW_BITWISE_FLAG_OPERATIONS(MainWindowView::Flag)

} // namespace graphos

#endif // GRAPHOS_MAINWINDOW_VIEW_H
