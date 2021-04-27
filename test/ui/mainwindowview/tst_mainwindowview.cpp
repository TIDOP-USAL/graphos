#include <QtTest>
#include <QCoreApplication>

#include "inspector/ui/MainWindowView.h"
#include "inspector/widgets/ThumbnailsWidget.h"

#include <QTreeWidgetItem>
#include <QAction>
#include <QMenu>

using namespace inspector;

class TestMainWindowView
  : public ui::MainWindowView
{
  Q_OBJECT

public:

  TestMainWindowView();
  ~TestMainWindowView();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_setProjectTitle();
  void test_setFlag();
  void test_clear();
  void test_updateHistory();
  void test_deleteHistory();

// signals:

  /* Menu File */

  void test_openFromHistory();
  void test_clearHistory();
  void test_saveProject();
  void test_saveProjectAs();
  void test_closeProject();
  void test_exit();

  /* Menú View */

  void test_openStartPage();

  /* Menú flujo de trabajo */

  void test_newProcessing();
  void test_openPreprocess();
  void test_openFeatureExtraction();
  void test_openFeatureMatching();

  /* Menú herramientas */

  void test_openCameras();
  void test_openSettings();

  /* Menú Ayuda */

  void test_openHelpDialog();
  void test_openAboutDialog();

  /* Panel de vistas en miniatura */

  void test_openImage();
  void test_selectImage();
  void test_selectImages();
  void test_deleteImages();
  void test_deleteImage();
};

TestMainWindowView::TestMainWindowView()
  : ui::MainWindowView()
{

}

TestMainWindowView::~TestMainWindowView()
{

}

void TestMainWindowView::initTestCase()
{
   QStringList history{
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba1\\Prueba1.xml"),
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba2\\Prueba2.xml"),
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba3\\Prueba3.xml")
  };
  this->updateHistory(history);

  /// Se simula la carga de un proyecto
  this->setProjectTitle("Example");
  setFlag(MainWindowView::Flag::project_exists, true);
  QStringList images {"C:\\img01.jpg", "C:\\img02.jpg"};
  addImages(images);
}

void TestMainWindowView::cleanupTestCase()
{

}

void TestMainWindowView::test_setProjectTitle()
{
  this->setProjectTitle("Example");
  QCOMPARE("Inspector Image - Example", this->windowTitle());
  QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0);
  QCOMPARE("Project: Example", itemProject->text(0));
}

void TestMainWindowView::test_setFlag()
{

  /// proyecto cargado
  this->setFlag(MainWindowView::Flag::project_exists, true);
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(false, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(false, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(false, mActionExportTiePoints->isEnabled());
  QCOMPARE(false, mActionExportMatches->isEnabled());

//  QCOMPARE(true, mActionViewSettings->isEnabled());
//  QCOMPARE(true, mActionHelp->isEnabled());
//  QCOMPARE(true, mActionAbout->isEnabled());


  /// Projecto modificado
  this->setFlag(MainWindowView::Flag::project_modified, true);
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(true, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(false, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(false, mActionExportTiePoints->isEnabled());
  QCOMPARE(false, mActionExportMatches->isEnabled());

  /// Imagenes añadidas
  setFlag(MainWindowView::Flag::images_added, true);
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(true, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(true, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(false, mActionExportTiePoints->isEnabled());
  QCOMPARE(false, mActionExportMatches->isEnabled());

  /// Extracción de caracteristicas
  setFlag(MainWindowView::Flag::feature_extraction, true); 
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(true, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(true, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(true, mActionExportTiePoints->isEnabled());
  QCOMPARE(false, mActionExportMatches->isEnabled());

  ///Matching
  setFlag(MainWindowView::Flag::feature_matching, true);
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(true, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(true, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(true, mActionExportTiePoints->isEnabled());
  QCOMPARE(true, mActionExportMatches->isEnabled());

  /// Orientación
  setFlag(MainWindowView::Flag::oriented, true);
  QCOMPARE(true, mActionNewProject->isEnabled());
  QCOMPARE(true, mActionOpenProject->isEnabled());
  QCOMPARE(true, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(true, mActionSaveProject->isEnabled());
  QCOMPARE(true, mActionSaveProjectAs->isEnabled());
  QCOMPARE(true, mActionCloseProject->isEnabled());
  QCOMPARE(true, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(true, mActionLoadImages->isEnabled());
  QCOMPARE(true, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(true, mActionExportTiePoints->isEnabled());
  QCOMPARE(true, mActionExportMatches->isEnabled());

  setFlag(MainWindowView::Flag::processing, true);
  QCOMPARE(false, mActionNewProject->isEnabled());
  QCOMPARE(false, mActionOpenProject->isEnabled());
  QCOMPARE(false, mMenuRecentProjects->isEnabled());
  QCOMPARE(false, mActionSaveProject->isEnabled());
  QCOMPARE(false, mActionSaveProjectAs->isEnabled());
  QCOMPARE(false, mActionCloseProject->isEnabled());
  QCOMPARE(false, mActionExit->isEnabled());
  QCOMPARE(false, mMenuRecentProjects->isEnabled());

  QCOMPARE(false, mActionNewProject->isEnabled());
  QCOMPARE(false, mActionOpenProject->isEnabled());
  QCOMPARE(false, mMenuRecentProjects->isEnabled());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(false, mActionSaveProject->isEnabled());
  QCOMPARE(false, mActionSaveProjectAs->isEnabled());
  QCOMPARE(false, mActionCloseProject->isEnabled());
  QCOMPARE(false, mActionExit->isEnabled());
  QCOMPARE(true, mActionStartPage->isEnabled());
  QCOMPARE(false, mActionLoadImages->isEnabled());
  QCOMPARE(true, mActionCameras->isEnabled());
  QCOMPARE(true, mActionSettings->isEnabled());
  QCOMPARE(false, mActionExportTiePoints->isEnabled());
  QCOMPARE(false, mActionExportMatches->isEnabled());

}

void TestMainWindowView::test_clear()
{
  this->clear();
  QCOMPARE("Inspector Image", this->windowTitle());

  QCOMPARE(false, mFlags.isActive(Flag::project_exists));
  QCOMPARE(false, mFlags.isActive(Flag::project_exists));
  QCOMPARE(false, mFlags.isActive(Flag::project_modified));
  QCOMPARE(false, mFlags.isActive(Flag::images_added));
  QCOMPARE(false, mFlags.isActive(Flag::image_open));
  QCOMPARE(false, mFlags.isActive(Flag::feature_extraction));
  QCOMPARE(false, mFlags.isActive(Flag::feature_matching));
}

void TestMainWindowView::test_updateHistory()
{
  QCOMPARE(3, mHistory.size());
  QCOMPARE(false, mActionNotRecentProjects->isVisible());
  QCOMPARE(true, mActionClearHistory->isEnabled());
  QCOMPARE(QString("&1 Prueba1.xml"), mHistory[0]->text());
  QCOMPARE(QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba1\\Prueba1.xml"), mHistory[0]->data());
  QCOMPARE(QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba1\\Prueba1.xml"), mHistory[0]->toolTip());
}

void TestMainWindowView::test_deleteHistory()
{
   this->deleteHistory();

  QCOMPARE(0, mHistory.size());
  QCOMPARE(true, mActionNotRecentProjects->isVisible());
  QCOMPARE(false, mActionClearHistory->isEnabled());

  /// Se recupera el historial para evitar problemas con otras pruebas
  QStringList history{
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba1\\Prueba1.xml"),
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba2\\Prueba2.xml"),
    QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba3\\Prueba3.xml")
  };
  this->updateHistory(history);
}

void TestMainWindowView::test_openFromHistory()
{
  QSignalSpy spy_openProjectFromHistory(this, &MainWindowView::openProjectFromHistory);

  emit mHistory[0]->triggered(true);
  QCOMPARE(spy_openProjectFromHistory.count(), 1);
  QList<QVariant> args = spy_openProjectFromHistory.takeFirst();
  QCOMPARE(args.at(0).toString(), QString("C:\\Users\\Tido\\Documents\\inspector\\Projects\\Prueba1\\Prueba1.xml"));
}

void TestMainWindowView::test_clearHistory()
{
  QSignalSpy spy_clearHistory(this, &MainWindowView::clearHistory);

  emit mActionClearHistory->triggered(true);
  QCOMPARE(spy_clearHistory.count(), 1);
}

void TestMainWindowView::test_saveProject()
{
  QSignalSpy spy_saveProject(this, &MainWindowView::saveProject);

  emit mActionSaveProject->triggered(true);
  QCOMPARE(spy_saveProject.count(), 1);
}

void TestMainWindowView::test_saveProjectAs()
{
  QSignalSpy spy_saveProjectAsy(this, &MainWindowView::saveProjectAs);

  emit mActionSaveProjectAs->triggered(true);
  QCOMPARE(spy_saveProjectAsy.count(), 1);
}

void TestMainWindowView::test_closeProject()
{
  QSignalSpy spy_closeProject(this, &MainWindowView::closeProject);

  emit mActionCloseProject->triggered(true);
  QCOMPARE(spy_closeProject.count(), 1);
}

void TestMainWindowView::test_exit()
{
  QSignalSpy spy_exit(this, &MainWindowView::exit);

  emit mActionExit->triggered(true);
  QCOMPARE(spy_exit.count(), 1);
}

void TestMainWindowView::test_openStartPage()
{
  QSignalSpy spy_openStartPage(this, &MainWindowView::openStartPage);

  emit mActionStartPage->triggered(true);
  QCOMPARE(spy_openStartPage.count(), 1);
}

void TestMainWindowView::test_newProcessing()
{
//  QSignalSpy spy_newProcessing(this, &MainWindowView::newSession);
//
//  emit mActionNewSession->triggered(true);
//  QCOMPARE(spy_newProcessing.count(), 1);
}

void TestMainWindowView::test_openPreprocess()
{
//  QSignalSpy spy_openPreprocess(this, &MainWindowView::openPreprocess);
//
//  emit mActionPreprocess->triggered(true);
//  QCOMPARE(spy_openPreprocess.count(), 1);
}

void TestMainWindowView::test_openFeatureExtraction()
{
//  QSignalSpy spy_openFeatureExtraction(this, &MainWindowView::openFeatureExtraction);
//
//  emit mActionFeatureExtraction->triggered(true);
//  QCOMPARE(spy_openFeatureExtraction.count(), 1);
}

void TestMainWindowView::test_openFeatureMatching()
{
//  QSignalSpy spy_openFeatureMatching(this, &MainWindowView::openFeatureMatching);
//
//  emit mActionFeatureMatching->triggered(true);
  //  QCOMPARE(spy_openFeatureMatching.count(), 1);
}

void TestMainWindowView::test_openCameras()
{
  QSignalSpy spy_openCamerasDialog(this, &MainWindowView::openCamerasDialog);

  emit mActionCameras->triggered(true);

  QCOMPARE(spy_openCamerasDialog.count(), 1);
}

void TestMainWindowView::test_openSettings()
{
  QSignalSpy spy_openSettings(this, &MainWindowView::openSettings);

  emit mActionSettings->triggered(true);
  QCOMPARE(spy_openSettings.count(), 1);
}

void TestMainWindowView::test_openHelpDialog()
{
//  QSignalSpy spy_openHelpDialog(this, &MainWindowView::openHelpDialog);
//
//  emit mActionHelp->triggered(true);
//  QCOMPARE(spy_openHelpDialog.count(), 1);
}

void TestMainWindowView::test_openAboutDialog()
{
//  QSignalSpy spy_openAboutDialog(this, &MainWindowView::openAboutDialog);
//
//  emit mActionAbout->triggered(true);
//  QCOMPARE(spy_openAboutDialog.count(), 1);
}

void TestMainWindowView::test_openImage()
{
//  QSignalSpy spy_openImage(this, &MainWindowView::openImage);
//  emit mThumbnailsWidget->openImage("C:\\img01.jpg");
//  QCOMPARE(spy_openImage.count(), 1);
//  QList<QVariant> args = spy_openImage.takeFirst();
//  QCOMPARE(args.at(0).toString(), QString("C:\\img01.jpg"));
}

void TestMainWindowView::test_selectImage()
{
//  QSignalSpy spy_selectImage(this, &MainWindowView::selectImage);
//  emit mThumbnailsWidget->selectImage("C:\\img01.jpg");
//  QCOMPARE(spy_selectImage.count(), 1);
//  QList<QVariant> args = spy_selectImage.takeFirst();
//  QCOMPARE(args.at(0).toString(), QString("C:\\img01.jpg"));
//
//  /// la señal selectImage es escuchada desde MainWindowPresenter y se
//  /// establece la imagen activa mediante setImageActive
//  setActiveImage("C:\\img02.jpg");
//  QCOMPARE(spy_selectImage.count(), 0);
}

void TestMainWindowView::test_selectImages()
{

}

void TestMainWindowView::test_deleteImages()
{
//  QSignalSpy spy_deleteImages(this, &MainWindowView::deleteImages);
//  QStringList images {"C:\\img01.jpg", "C:\\img02.jpg"};
//  emit mThumbnailsWidget->deleteImages(images);
//  QCOMPARE(spy_deleteImages.count(), 1);
//  QList<QVariant> args = spy_deleteImages.takeFirst();
//  QCOMPARE(args.at(0).toStringList(), images);
}

void TestMainWindowView::test_deleteImage()
{

}

QTEST_MAIN(TestMainWindowView)

#include "tst_mainwindowview.moc"
