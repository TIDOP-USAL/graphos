#ifndef INSPECTOR_MAINWINDOW_VIEW_H
#define INSPECTOR_MAINWINDOW_VIEW_H

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

namespace inspector
{

class ThumbnailsWidget;
class LogWidget;


namespace ui
{

class GraphicViewer;
class TabHandler;

class MainWindowView
  : public QMainWindow
{
  Q_OBJECT

public:

  enum class Flag
  {
    project_exists        = (1 << 0),  // Existe un proyecto
    project_modified      = (1 << 1),  // Se ha modificado el proyecto
    images_added          = (1 << 2),  // Se han añadido fotogramas
    image_open            = (1 << 3),  // Hay una imagen abierta
    session_created       = (1 << 4),
    preprocess            = (1 << 5),
    feature_extraction    = (1 << 6),
    feature_matching      = (1 << 7),
    processing            = (1 << 20),
    loading_images        = (1 << 21),
    ground_truth          = (1 << 30),
  };

public:

  explicit MainWindowView(QWidget *parent = nullptr);
  ~MainWindowView() override;

  void clear();

  /*!
   * \brief Establece el nombre del proyecto
   * \param[in] title Titulo del proyecto
   */
  void setProjectTitle(const QString &title);

  void setFlag(Flag flag, bool value);
  void addImages(const QStringList &images);
  void setActiveImage(const QString &image);
  void setActiveImages(const QStringList &images);
  void addFeatures(const QString &sessionName, const QString &detector, const QString &descriptor, const std::vector<QString> &features);
  void addMatches(const QString &sessionName, const QString &matcher, const QString &left, const QString &right, const QString &file);

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
  TabHandler *tabHandler();

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
  void deleteImage(const QString &file);
  void deleteFeatures(const QString &session, const QString &feat);
  void deleteMatches(const QString &session, const QString &matches);

signals:

  /* Menu File */

  void openNew();
  void openProject();
  void openProjectFromHistory(QString);
  void clearHistory();
  void saveProject();
  void saveProjectAs();
  void openExportFeatures();
  void openExportMatches();
  void closeProject();
  void exit();
  void openSettings();

  /* Menú View */

  void openStartPage();
  void openViewSettings();

  /* Menú herramientas */

  void loadImages();
  void openFeatureExtraction();
  void openFeatureMatching();
  void openBatch();
  void openToolSettings();

  /* Quality Control */

  void openKeypointsViewer();
  void openMatchesViewer();
  void openMultiviewMatchingAssessment();

  /* Menú Ayuda */

  void openHelpDialog();
  void openAboutDialog();

  /* Panel de vistas en miniatura */

  ///TODO: asegurarse que se utiliza siempre el mismo criterio como id de la imagen
  ///      O nombre de la imagen o ruta completa
  void openImage(QString);
  void selectImage(QString);
  void selectImages(QStringList);
  void deleteImages(QStringList);
  void selectFeatures(QString);
  void selectDetector(QString);
  void selectDescriptor(QString);
  void selectImageFeatures(QString);
  void imagesLoaded();

  void openImageMatches(QString, QString, QString);

  void openKeypointsViewer(QString, QString);
  void openMatchesViewer(QString, QString, QString);
  void openMultiView(QString);

protected:

  void changeEvent(QEvent *e) override;

private slots:

  void update();
  void retranslate();

  void openFromHistory();
  void onSelectionChanged();
  void onItemDoubleClicked(QTreeWidgetItem *item, int column);
  void onTreeContextMenu(const QPoint &point);

private:

  void initUI();
  void initSignalAndSlots();

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
  QAction *mActionFeatureExtraction;
  QAction *mActionFeatureMatching;
  QAction *mActionOrientation;
  QAction *mActionDensification;
  QAction *mActionViewSettings;
  QAction *mActionToolSettings;
  QAction *mActionHelp;
  QAction *mActionAbout;
  QAction *mActionExportTiePoints;
  QAction *mActionExportMatches;
  QAction *mActionFeaturesViewer;
  QAction *mActionMatchesViewer;
  QAction *mActionPassPointsViewer;
  QAction *mActionNotRecentProjects;
  QAction *mActionClearHistory;
  QAction *mActionZoomIn;
  QAction *mActionZoomOut;
  QAction *mActionZoomExtend;
  QAction *mActionZoom11;
  QMenu *mMenuRecentProjects;
  QMenu *mMenuExport;
  QMenu *mMenuPanels;
  QMenu *mMenuToolBar;
  ThumbnailsWidget *mThumbnailsWidget;
  LogWidget *mLogWidget;
  QWidget *mStartPageWidget;
  QComboBox *mComboBoxActiveSession;
  QProgressBar *mProgressBar;

  tl::EnumFlags<Flag> mFlags;
  std::vector<QAction*> mHistory;
  QGridLayout *mLayoutCentral;
  TabHandler *mTabHandler;

private:

  Ui::MainWindowView *ui;
};

ALLOW_BITWISE_FLAG_OPERATIONS(MainWindowView::Flag)

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MAINWINDOW_VIEW_H
