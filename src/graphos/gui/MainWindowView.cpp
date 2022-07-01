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

#include "MainWindowView.h"
#include "ui_MainWindowView.h"

#include "graphos/widgets/ThumbnailsWidget.h"
#include "graphos/widgets/LogWidget.h"
#include "graphos/widgets/TabWidget.h"
#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/Viewer3d.h"
#include "graphos/widgets/ProgressBarWidget.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/messages.h>

#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QSettings>
#include <QGraphicsEllipseItem>
#include <QDesktopServices>
#include <QUrl>
#include <QComboBox>
#include <QProgressBar>
#include <QLabel>
#include <QToolBar>

namespace graphos
{

enum
{
  project,
  images,
  image,                   // Imagen
  image_features,          // Imagen + features
  image_features_matches,  // Imagen + features + matches
//  features,
//  features_images,
//  features_image,
  detector,
  descriptor,
  matches,
  descriptor_matcher,
  matches_pairs,
  pair_left,
  pair_right,
  sparse_model,
  dense_model,
  dsm,
  ortho
};

MainWindowView::MainWindowView(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindowView),
    mActionNewProject(new QAction(this)),
    mActionOpenProject(new QAction(this)),
    mActionSaveProject(new QAction(this)),
    mActionSaveProjectAs(new QAction(this)),
    mActionCloseProject(new QAction(this)),
    mActionExit(new QAction(this)),
    mActionStartPage(new QAction(this)),
    mActionCameras(new QAction(this)),
    mActionHelp(new QAction(this)),
    mActionImportCameras(new QAction(this)),
    mActionExportTiePoints(new QAction(this)),
    mActionExportMatches(new QAction(this)),
    mActionExportPointCloud(new QAction(this)),
    mActionOrtho(new QAction(this)),
    mActionNotRecentProjects(new QAction(this)),
    mActionClearHistory(new QAction(this)),
    mActionOpenImage(new QAction(this)),
    mActionDeleteImage(new QAction(this)),
    mActionViewKeypoints(new QAction(this)),
    mActionViewMatches(new QAction(this)),
    mActionOpenModel3D(new QAction(this)),
    mActionZoomIn(new QAction(this)),
    mActionZoomOut(new QAction(this)),
    mActionZoomExtend(new QAction(this)),
    mActionZoom11(new QAction(this)),
    mActionGlobalZoom(new QAction(this)),
    mActionViewFront(new QAction(this)),
    mActionViewTop(new QAction(this)),
    mActionViewLeft(new QAction(this)),
    mActionViewRight(new QAction(this)),
    mActionViewBack(new QAction(this)),
    mActionViewBottom(new QAction(this)),
    mActionPointMeasuse(new QAction(this)),
    mActionDistanceMeasuse(new QAction(this)),
    mActionAngleMeasure(new QAction(this))
{
  ui->setupUi(this);

  this->initUI();
  this->initSignalAndSlots();

  Application &app = Application::instance();
  AppStatus *app_status = app.status();
  connect(app_status, &AppStatus::update,
          this, &MainWindowView::update);
}

MainWindowView::~MainWindowView()
{
  delete ui;
}

void MainWindowView::setCreateProjectAction(QAction *action)
{
  ui->menuFile->insertAction(mActionNewProject, action);
  ui->menuFile->removeAction(mActionNewProject);
  mToolBarFile->insertAction(mActionNewProject, action);
  mToolBarFile->removeAction(mActionNewProject);
}

void MainWindowView::setOpenProjectAction(QAction *action)
{
  ui->menuFile->insertAction(mActionOpenProject, action);
  ui->menuFile->removeAction(mActionOpenProject);
  mToolBarFile->insertAction(mActionOpenProject, action);
  mToolBarFile->removeAction(mActionOpenProject);
}
void MainWindowView::setImportCamerasAction(QAction *action)
{
  mMenuImport->insertAction(mActionImportCameras, action);
  mMenuImport->removeAction(mActionImportCameras);
}

void MainWindowView::setCamerasToolAction(QAction* action)
{
  ui->menuTools->insertAction(mActionCameras, action);
  ui->menuTools->removeAction(mActionCameras);
  mToolBarTools->insertAction(mActionCameras, action);
  mToolBarTools->removeAction(mActionCameras);
}

void MainWindowView::addActionToMenu(QAction *action, Menu menuName)
{
  if (QMenu *_menu = findMenu(menuName)) {
    _menu->addAction(action);
  }
}

void MainWindowView::addMenuToMenu(QMenu *menu, Menu menuName)
{
  if (QMenu *_menu = findMenu(menuName)) {
    _menu->addMenu(menu);
  }
}

void MainWindowView::addSeparatorToMenu(Menu menu)
{
  if (QMenu *_menu = findMenu(menu)) {
    _menu->addSeparator();
    //this->update();
  }
}

void MainWindowView::addActionToToolbar(QAction *action, Toolbar toolbar)
{
  if (QToolBar *_toolbar = findToolbar(toolbar)) {
    _toolbar->addAction(action);
    //this->update();
  }
}

void MainWindowView::addSeparatorToToolbar(Toolbar toolbar)
{
  if (QToolBar *_toolbar = findToolbar(toolbar)) {
    _toolbar->addSeparator();
    //this->update();
  }
}

void MainWindowView::clear()
{
  setWindowTitle(QString("Graphos"));
  const QSignalBlocker blockerTreeWidgetProject(mTreeWidgetProject);
  mTreeWidgetProject->clear();
  const QSignalBlocker blockerThumbnailsWidget(mThumbnailsWidget);
  mThumbnailsWidget->clear();
  ui->treeWidgetProperties->clear();
  mFlags.clear();

  if (mTabWidget) mTabWidget->clear();

  update();
}

void MainWindowView::setProjectTitle(const QString &title)
{
  this->setWindowTitle(QString("Graphos - ").append(title));

  QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0);
  if (itemProject == nullptr) {
    itemProject = new QTreeWidgetItem();
    mTreeWidgetProject->addTopLevelItem(itemProject);
    itemProject->setExpanded(true);
    itemProject->setData(0, Qt::UserRole, graphos::project);
  }

  itemProject->setText(0, tr("Project: ").append(title));
}

void MainWindowView::setFlag(MainWindowView::Flag flag, bool value)
{
  mFlags.activeFlag(flag, value);
  update();
}

void MainWindowView::addImage(const QString &image, size_t imageId)
{

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages == nullptr) {
      itemImages = new QTreeWidgetItem();
      itemImages->setText(0, tr("Images"));
      itemImages->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
      itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
      itemImages->setData(0, Qt::UserRole, graphos::images);
      itemProject->addChild(itemImages);
      itemImages->setExpanded(true);
    }

    /* Se añade el fotograma al árbol del proyecto */
    QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
    itemPhotogram->setText(0, QFileInfo(image).baseName());
    itemPhotogram->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-image-file.png"));
    itemPhotogram->setToolTip(0, image);
    itemPhotogram->setData(0, Qt::UserRole, graphos::image);
    itemPhotogram->setData(0, Qt::UserRole+1, imageId);
    itemImages->addChild(itemPhotogram);
    itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));

    update();
  }

  mThumbnailsWidget->addThumbnail(image, imageId);
}

//void MainWindowView::addImages(const QStringList &images)
//{
//  if (images.empty() == false) {
//    for (auto &image : images) {
//      if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {
//
//        /* Fotogramas */
//
//        QTreeWidgetItem *itemImages = nullptr;
//        for (int i = 0; i < itemProject->childCount(); i++) {
//          QTreeWidgetItem *temp = itemProject->child(i);
//          //if (temp->text(0).compare(tr("Images")) == 0) {
//          if (temp->data(0, Qt::UserRole) == graphos::images){
//            itemImages = itemProject->child(i);
//            break;
//          }
//        }
//
//        if (itemImages == nullptr) {
//          itemImages = new QTreeWidgetItem();
//          itemImages->setText(0, tr("Images"));
//          itemImages->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
//          itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
//          itemImages->setData(0, Qt::UserRole, graphos::images);
//          itemProject->addChild(itemImages);
//          itemImages->setExpanded(true);
//        }
//
//        /* Se añade el fotograma al árbol del proyecto */
//        QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
//        itemPhotogram->setText(0, QFileInfo(image).baseName());
//        itemPhotogram->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-image-file.png"));
//        itemPhotogram->setToolTip(0, image);
//        itemPhotogram->setData(0, Qt::UserRole, graphos::image);
//        itemImages->addChild(itemPhotogram);
//        itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));
//        
//        update();
//      }
//    }
//
//    mThumbnailsWidget->addThumbnails(images);
//
//  }
//}

void MainWindowView::setActiveImage(size_t imageId)
{
  const QSignalBlocker blocker1(mTreeWidgetProject);
  const QSignalBlocker blocker2(mThumbnailsWidget);

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        size_t id = temp->data(0, Qt::UserRole + 1).toULongLong();
        itemImages->child(i)->setSelected(id == imageId);
      }
    }
  }

  mThumbnailsWidget->setActiveImage(imageId);

}

void MainWindowView::setActiveImages(const std::vector<size_t> &imageIds)
{
  const QSignalBlocker blocker1(mTreeWidgetProject);
  const QSignalBlocker blocker2(mThumbnailsWidget);

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        temp->setSelected(false);
        for (auto id : imageIds) {
          if (temp->data(0, Qt::UserRole + 1).toULongLong() == id) {
            temp->setSelected(true);
            break;
          }
        }
      }
    }
  }

  mThumbnailsWidget->setActiveImages(imageIds);
}

void MainWindowView::addFeatures(size_t imageId)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->data(0, Qt::UserRole+1).toULongLong() == imageId){
          itemImage = itemImages->child(i);
          break;
        }
      }
      if (itemImage)
        itemImage->setData(0, Qt::UserRole, graphos::image_features);
    }
  }
}

void MainWindowView::deleteFeatures(size_t imageId)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId){
          itemImage = itemImages->child(i);
          break;
        }
      }
      if (itemImage)
        itemImage->setData(0, Qt::UserRole, graphos::image);
    }
  }
}

void MainWindowView::addMatches(size_t imageId)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId){
          itemImage = itemImages->child(i);
          break;
        }
      }

      itemImage->setData(0, Qt::UserRole, graphos::image_features_matches);
    }
  }
}

void MainWindowView::deleteMatches(size_t imageId)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId){
          itemImage = itemImages->child(i);
          break;
        }
      }

      itemImage->setData(0, Qt::UserRole, graphos::image_features);
    }
  }
}

void MainWindowView::setSparseModel(const QString &sparseModel)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemModels = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->text(0).compare(tr("3D Models")) == 0) {
        itemModels = itemProject->child(i);
        break;
      }
    }

    if (itemModels == nullptr) {
      itemModels = new QTreeWidgetItem();
      itemModels->setText(0, tr("3D Models"));
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemSparseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("Sparse Model")) == 0) {
        itemSparseModel = temp;
        break;
      }
    }

    if (itemSparseModel == nullptr){
      itemSparseModel = new QTreeWidgetItem();
      itemModels->addChild(itemSparseModel);
    }

    itemSparseModel->setText(0, "Sparse Model");
    itemSparseModel->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
    itemSparseModel->setToolTip(0, sparseModel);
    itemSparseModel->setData(0, Qt::UserRole, graphos::sparse_model);
  }
}

void MainWindowView::deleteSparseModel()
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemModels = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->text(0).compare(tr("3D Models")) == 0) {
        itemModels = itemProject->child(i);
        break;
      }
    }

    if (itemModels == nullptr) {
      itemModels = new QTreeWidgetItem();
      itemModels->setText(0, tr("3D Models"));
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemSparseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("Sparse Model")) == 0) {
        itemSparseModel = temp;
        delete itemSparseModel;
        itemSparseModel = nullptr;
        break;
      }
    }
  }
}

void MainWindowView::setDenseModel(const QString &denseModel)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemModels = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->text(0).compare(tr("3D Models")) == 0) {
        itemModels = itemProject->child(i);
        break;
      }
    }

    if (itemModels == nullptr) {
      itemModels = new QTreeWidgetItem();
      itemModels->setText(0, tr("3D Models"));
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemDenseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("Dense Model")) == 0) {
        itemDenseModel = temp;
        break;
      }
    }

    if (itemDenseModel == nullptr){
      itemDenseModel = new QTreeWidgetItem();
      itemModels->addChild(itemDenseModel);
    }

    itemDenseModel->setText(0, "Dense Model");
    itemDenseModel->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
    itemDenseModel->setToolTip(0, denseModel);
    itemDenseModel->setData(0, Qt::UserRole, graphos::dense_model);
  }
}

void MainWindowView::deleteDenseModel()
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemModels = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->text(0).compare(tr("3D Models")) == 0) {
        itemModels = itemProject->child(i);
        break;
      }
    }

    if (itemModels == nullptr) {
      itemModels = new QTreeWidgetItem();
      itemModels->setText(0, tr("3D Models"));
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemDenseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("Dense Model")) == 0) {
        itemDenseModel = temp;
        delete itemDenseModel;
        itemDenseModel = nullptr;
        break;
      }
    }

  }
}

void MainWindowView::setDSM(const QString &dsm)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem* itemDSM = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem* temp = itemProject->child(i);
      if (temp->text(0).compare(tr("DTM/DSM")) == 0) {
        itemDSM = temp;
        break;
      }
    }

    if (itemDSM == nullptr) {
      itemDSM = new QTreeWidgetItem();
      itemProject->addChild(itemDSM);
    }

    itemDSM->setText(0, "DTM/DSM");
    itemDSM->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-image-file.png"));
    itemDSM->setToolTip(0, dsm);
    itemDSM->setData(0, Qt::UserRole, graphos::dsm);
  }
}

void MainWindowView::deleteDsm()
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemDSM = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem* temp = itemProject->child(i);
      if (temp->text(0).compare(tr("DTM/DSM")) == 0) {
        itemDSM = temp;
        delete itemDSM;
        itemDSM = nullptr;
        break;
      }
    }
  }
}

void MainWindowView::setOrtho(const QString& ortho)
{
}

void MainWindowView::deleteOrtho()
{
}

void MainWindowView::setStatusBarMsg(const QString &msg)
{
  ui->statusBar->showMessage(msg, 2000);
}

void MainWindowView::setProperties(const std::list<std::pair<QString, QString>> &properties)
{
  ui->treeWidgetProperties->clear();
  ui->treeWidgetProperties->setAlternatingRowColors(true);

  for (auto it = properties.begin(); it != properties.end(); it++){
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, it->first);
    item->setText(1, it->second);
    ui->treeWidgetProperties->addTopLevelItem(item);
  }
}

ProgressBarWidget *MainWindowView::progressBar()
{
  return mProgressBar;
}


TabWidget *MainWindowView::tabWidget()
{
  return mTabWidget;
}

void MainWindowView::updateHistory(const QStringList &history)
{
  int n = history.size();
  for (int r = 0; r < n; r++) {
    QString prjFileName = tr("&%1 %2").arg(r + 1).arg(QFileInfo(history[r]).fileName());
    if (mHistory.size() == static_cast<size_t>(r)) {
      // Se añade un nuevo elemento
      QAction *action = new QAction(prjFileName, this);
      action->setData(history[r]);
      action->setToolTip(history[r]);
      mHistory.push_back(action);
      connect(mHistory[static_cast<size_t>(r)], SIGNAL(triggered()), this, SLOT(openFromHistory()));
      mMenuRecentProjects->insertAction(mActionNotRecentProjects, mHistory[static_cast<size_t>(r)]);
      mMenuRecentProjects->setToolTipsVisible(true);
    } else {
      mHistory[static_cast<size_t>(r)]->setText(prjFileName);
      mHistory[static_cast<size_t>(r)]->setData(history[r]);
      mHistory[static_cast<size_t>(r)]->setToolTip(history[r]);
    }
  }

  update();
}

void MainWindowView::deleteHistory()
{
  while (mHistory.size() > 0) {
    disconnect(mHistory[0], SIGNAL(triggered()), this, SLOT(openFromHistory()));
    mMenuRecentProjects->removeAction(mHistory[0]);
    mHistory.erase(mHistory.begin());
  }

  update();
}

void MainWindowView::deleteImages(const std::vector<size_t> &imageIds)
{
  const QSignalBlocker blocker1(mTreeWidgetProject);
  const QSignalBlocker blocker2(mThumbnailsWidget);

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      if (temp->data(0, Qt::UserRole) == graphos::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {

      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        for (auto imageId : imageIds) {
          if (temp && temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
            itemImage = itemImages->child(i);
            delete itemImage;
            itemImage = nullptr;
            i--;
            break;
          }
        }
      }     

      itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));
    }

    setFlag(MainWindowView::Flag::images_added, itemProject->childCount() > 0);
  }

  mThumbnailsWidget->deleteImages(imageIds);
}

void MainWindowView::changeEvent(QEvent *e)
{
  QMainWindow::changeEvent(e);
  switch (e->type()) {
  case QEvent::LanguageChange:
    ui->retranslateUi(this);
    this->retranslate();
    break;
  default:
    break;
  }
}



void MainWindowView::openFromHistory()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    emit openProjectFromHistory(action->data().toString());
}

void MainWindowView::onSelectionChanged()
{
  QList<QTreeWidgetItem*> item = mTreeWidgetProject->selectedItems();
  if (item.isEmpty()) return;
  if (item[0]->data(0, Qt::UserRole) == graphos::project){

  } else if (item[0]->data(0, Qt::UserRole) == graphos::images){
  } else if (item[0]->data(0, Qt::UserRole) == graphos::image ||
             item[0]->data(0, Qt::UserRole) == graphos::image_features ||
             item[0]->data(0, Qt::UserRole) == graphos::image_features_matches){
    int size = item.size();
    if(size > 0){
      if (size == 1) {
        emit selectImage(item[0]->data(0, Qt::UserRole+1).toULongLong());
      } else {
        std::vector<size_t> selected_images;
        for (int i = 0; i < size; i++){
          selected_images.push_back(item[i]->data(0, Qt::UserRole+1).toULongLong());
        }
        emit selectImages(selected_images);
      }
    }
  }
}

void MainWindowView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
  if (item){
    if (item->data(0, Qt::UserRole) == graphos::image ||
        item->data(0, Qt::UserRole) == graphos::image_features ||
        item->data(0, Qt::UserRole) == graphos::image_features_matches){
     emit openImage(item->data(column, Qt::UserRole+1).toULongLong());
   } else if (item->data(0, Qt::UserRole) == graphos::sparse_model) {
     emit open3DModel(item->toolTip(column), true);
   } else if (item->data(0, Qt::UserRole) == graphos::dense_model) {
     emit open3DModel(item->toolTip(column), false);
   } else if (item->data(0, Qt::UserRole) == graphos::dsm) {
     emit openDtm();
   } else if (item->data(0, Qt::UserRole) == graphos::ortho) {
     emit openOrtho(item->toolTip(column));
   }

   /*else if (item->data(0, Qt::UserRole) == graphos::ui::pair_right){
     QString session = item->parent()->parent()->parent()->parent()->text(0);
     emit openImageMatches(session, item->parent()->text(0), item->text(column));
   } else if (item->data(0, Qt::UserRole) == graphos::ui::features_image){
     QString session = item->parent()->parent()->parent()->text(0);
     emit openKeypointsViewer(session, QFileInfo(item->text(column)).baseName());
   }*/
  }
}

void MainWindowView::onTreeContextMenu(const QPoint &point)
{
  QPoint globalPos = mTreeWidgetProject->mapToGlobal(point);

  QTreeWidgetItem *item = nullptr;
  item = mTreeWidgetProject->itemAt(point);

  if (item == nullptr)
    return;

  if (item->data(0, Qt::UserRole) == graphos::project){
    /*QAction *selectedItem = */mMenuTreeProjectImages->exec(globalPos);
  } else if (item->data(0, Qt::UserRole) == graphos::images){

  } else if (item->data(0, Qt::UserRole) == graphos::image ||
             item->data(0, Qt::UserRole) == graphos::image_features ||
             item->data(0, Qt::UserRole) == graphos::image_features_matches){

    if (item->data(0, Qt::UserRole) == graphos::image) {
      mActionViewKeypoints->setEnabled(false);
      mActionViewMatches->setEnabled(false);
    } else if (item->data(0, Qt::UserRole) == graphos::image_features) {
      mActionViewKeypoints->setEnabled(true);
      mActionViewMatches->setEnabled(false);
    } else {
      mActionViewKeypoints->setEnabled(true);
      mActionViewMatches->setEnabled(true);
    }

    if (QAction *selectedItem = mMenuTreeProjectImage->exec(globalPos)) {
      if (selectedItem->text() == tr("Open Image")) {
        emit openImage(item->data(0, Qt::UserRole+1).toULongLong());
      } else if (selectedItem->text() == tr("Delete Image")) {
        emit delete_images(std::vector<size_t>{item->data(0, Qt::UserRole+1).toULongLong()});
      } else if (selectedItem->text() == tr("View Keypoints")) {
        emit openKeypointsViewer(item->data(0, Qt::UserRole+1).toULongLong());
      } else if (selectedItem->text() == tr("View Matches")) {
        emit openMatchesViewer(item->data(0, Qt::UserRole + 1).toULongLong());
      }
    }
  } else if (item->data(0, Qt::UserRole) == graphos::sparse_model){
    if (QAction *selectedItem = mMenuTreeProjectModel3D->exec(globalPos)) {
      if (selectedItem->text() == tr("Open Point Cloud")) {
        emit open3DModel(item->toolTip(0), true);
      }
    }
  } else if (item->data(0, Qt::UserRole) == graphos::dense_model) {
    if (QAction *selectedItem = mMenuTreeProjectModel3D->exec(globalPos)) {
      if (selectedItem->text() == tr("Open Point Cloud")) {
        emit open3DModel(item->toolTip(0), false);
      }
    }
  }
}

void MainWindowView::initUI()
{
  setWindowTitle(QString("Graphos"));

  mLayoutCentral = new QGridLayout(this->centralWidget());
  mLayoutCentral->setSpacing(6);
  mLayoutCentral->setContentsMargins(0,0,0,0);

  this->initTabWidget();
  this->initThumbnailsTool();
  this->initConsole();

  this->initActions();
  this->initToolbars();
  this->initTreeWidget();
  this->initMenus();
  this->initProgressBar();

  // Configuración de mensajes
  tl::MessageManager &msg_h = tl::MessageManager::instance();
  msg_h.addListener(mLogWidget);

  this->retranslate();
  this->update();
}


void MainWindowView::initTabWidget()
{
  mTabWidget = new TabWidget(this->centralWidget());
  mLayoutCentral->addWidget(mTabWidget);
}

void MainWindowView::initThumbnailsTool()
{
  QGridLayout *gridLayoutThumb = new QGridLayout(ui->dockWidgetThumbContents);
  gridLayoutThumb->setSpacing(6);
  gridLayoutThumb->setContentsMargins(11, 11, 11, 11);
  gridLayoutThumb->setContentsMargins(0, 0, 0, 0);
  mThumbnailsWidget = new ThumbnailsWidget(ui->dockWidgetThumbContents);
  gridLayoutThumb->addWidget(mThumbnailsWidget, 0, 0, 1, 1);
}

void MainWindowView::initConsole()
{
  QGridLayout *gridLayoutConsole = new QGridLayout(ui->dockWidgetContentsConsole);
  gridLayoutConsole->setSpacing(6);
  gridLayoutConsole->setContentsMargins(11, 11, 11, 11);
  gridLayoutConsole->setContentsMargins(0, 0, 0, 0);
  mLogWidget = new LogWidget(ui->dockWidgetThumbContents);
  gridLayoutConsole->addWidget(mLogWidget, 0, 0, 1, 1);
}

void MainWindowView::initActions()
{
  QIcon iconNewProject;
  iconNewProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-empty-document.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionNewProject->setIcon(iconNewProject);

  QIcon iconOpenProject;
  iconOpenProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-open.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionOpenProject->setIcon(iconOpenProject);

  QIcon iconSaveProject;
  iconSaveProject.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-save.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionSaveProject->setIcon(iconSaveProject);

  QIcon iconSaveProjectAs;
  iconSaveProjectAs.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-save-as.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionSaveProjectAs->setIcon(iconSaveProjectAs);

  QIcon icon4;
  icon4.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-close-button.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionCloseProject->setIcon(icon4);

  mActionExit->setObjectName(QStringLiteral("actionExit"));
  QIcon iconExit;
  iconExit.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-switch-power-off.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionExit->setIcon(iconExit);

  QIcon iconStartPage;
  iconStartPage.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-home-page.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionStartPage->setIcon(iconStartPage);

  //QIcon iconSettings;
  //iconSettings.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-settings.png"), QSize(), QIcon::Normal, QIcon::Off);
  //mActionSettings->setIcon(iconSettings);

  QIcon iconHelp;
  iconHelp.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_help_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionHelp->setIcon(iconHelp);

  //QIcon iconAbout;
  //iconAbout.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_about_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  //mActionAbout->setIcon(iconAbout);

  mActionNotRecentProjects->setEnabled(false);

  QIcon icon_delete_trash;
  icon_delete_trash.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-delete-2.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionClearHistory->setIcon(icon_delete_trash);

  QIcon iconOpenImage;
  iconOpenImage.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_image_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionOpenImage->setIcon(iconOpenImage);

  QIcon iconRemoveImage;
  iconRemoveImage.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_remove_image_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionDeleteImage->setIcon(iconRemoveImage);

  mActionOpenModel3D->setIcon(QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
  mActionViewKeypoints->setIcon(QIcon(":/ico/24/img/material/24/view_points_24px.png"));
  mActionViewMatches->setIcon(QIcon(":/ico/24/img/material/24/view_match_24px.png"));

  QIcon iconZoomIn;
  iconZoomIn.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomIn->setIcon(iconZoomIn);

  QIcon iconZoomOut;
  iconZoomOut.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomOut->setIcon(iconZoomOut);

  QIcon iconZoomExtend;
  iconZoomExtend.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-magnifying-glass-with-expand-sign.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomExtend->setIcon(iconZoomExtend);

  QIcon iconZoom11;
  iconZoom11.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-one-to-one.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoom11->setIcon(iconZoom11);

  mActionGlobalZoom->setIcon(iconZoomExtend);

  QIcon iconViewFront;
  iconViewFront.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-front-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewFront->setIcon(iconViewFront);

  QIcon iconViewTop;
  iconViewTop.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-top-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewTop->setIcon(iconViewTop);

  QIcon iconViewLeft;
  iconViewLeft.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-cube.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewLeft->setIcon(iconViewLeft);

  QIcon iconViewRight;
  iconViewRight.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-view_right.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewRight->setIcon(iconViewRight);

  QIcon iconViewBack;
  iconViewBack.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-back-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewBack->setIcon(iconViewBack);

  QIcon iconViewBottom;
  iconViewBottom.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-bottom-view.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionViewBottom->setIcon(iconViewBottom);

  QIcon iconPointMeasuse;
  iconPointMeasuse.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-cursor.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionPointMeasuse->setIcon(iconPointMeasuse);
  mActionPointMeasuse->setCheckable(true);

  QIcon iconDistanceMeasuse;
  iconDistanceMeasuse.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_ruler_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionDistanceMeasuse->setIcon(iconDistanceMeasuse);
  mActionDistanceMeasuse->setCheckable(true);

  QIcon iconAngleMeasure;
  iconAngleMeasure.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_measurement_tool_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionAngleMeasure->setIcon(iconAngleMeasure);
  mActionAngleMeasure->setCheckable(true);
}

void MainWindowView::initToolbars()
{
  this->initToolbarFile();
  this->initToolbarWorkflow();
  this->initToolbarView();
  this->initToolbar3dModel();
  this->initToolbarTools();
}

void MainWindowView::initToolbarFile()
{
  mToolBarFile = new QToolBar(this);
  mToolBarFile->setObjectName("ToolBarFile");
  mToolBarFile->addAction(mActionNewProject);
  mToolBarFile->addAction(mActionOpenProject);
  mToolBarFile->addAction(mActionSaveProject);
  mToolBarFile->addAction(mActionSaveProjectAs);
  this->addToolBar(Qt::TopToolBarArea, mToolBarFile);
}

void MainWindowView::initToolbarWorkflow()
{
  mToolBarWorkflow = new QToolBar(this);
  mToolBarWorkflow->setObjectName("ToolBarWorkflow");
  this->addToolBar(Qt::TopToolBarArea, mToolBarWorkflow);
}

void MainWindowView::initToolbarView()
{
  mToolBarView = new QToolBar(this);
  mToolBarView->setObjectName("ToolBarView");
  mToolBarView->addAction(mActionZoomIn);
  mToolBarView->addAction(mActionZoomOut);
  mToolBarView->addAction(mActionZoomExtend);
  mToolBarView->addAction(mActionZoom11);
  this->addToolBar(Qt::TopToolBarArea, mToolBarView);
}

void MainWindowView::initToolbar3dModel()
{
  mToolBar3dModel = new QToolBar(this);
  mToolBar3dModel->setObjectName("ToolBar3dModel");
  mToolBar3dModel->addAction(mActionGlobalZoom);
  mToolBar3dModel->addSeparator();
  mToolBar3dModel->addAction(mActionViewFront);
  mToolBar3dModel->addAction(mActionViewBack);
  mToolBar3dModel->addAction(mActionViewTop);
  mToolBar3dModel->addAction(mActionViewBottom);
  mToolBar3dModel->addAction(mActionViewLeft);
  mToolBar3dModel->addAction(mActionViewRight);
  mToolBar3dModel->addSeparator();
  mToolBar3dModel->addAction(mActionPointMeasuse);
  mToolBar3dModel->addAction(mActionDistanceMeasuse);
  mToolBar3dModel->addAction(mActionAngleMeasure);
  this->addToolBar(Qt::TopToolBarArea, mToolBar3dModel);
}

void MainWindowView::initToolbarTools()
{
  mToolBarTools = new QToolBar(this);
  mToolBarTools->setObjectName("ToolBarTools");
  mToolBarTools->addAction(mActionCameras);
  this->addToolBar(Qt::TopToolBarArea, mToolBarTools);
}

void MainWindowView::initTreeWidget()
{
  mTreeWidgetProject = new QTreeWidget(ui->dockWidgetContentsProject);
  mTreeWidgetProject->setContextMenuPolicy(Qt::CustomContextMenu);
  mTreeWidgetProject->header()->close();
  mTreeWidgetProject->setColumnCount(1);
  mTreeWidgetProject->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  mTreeWidgetProject->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  mTreeWidgetProject->header()->setStretchLastSection(false);
  ui->gridLayout->addWidget(mTreeWidgetProject, 0, 0, 1, 1);

  mMenuTreeProjectImages = new QMenu(this);
  mMenuTreeProjectImage = new QMenu(this);
  mMenuTreeProjectImage->addAction(mActionOpenImage);
  mMenuTreeProjectImage->addAction(mActionDeleteImage);
  mMenuTreeProjectImage->addSeparator();
  mMenuTreeProjectImage->addAction(mActionViewKeypoints);
  mMenuTreeProjectImage->addSeparator();
  mMenuTreeProjectImage->addAction(mActionViewMatches);
  mMenuTreeProjectModel3D = new QMenu(this);
  mMenuTreeProjectModel3D->addAction(mActionOpenModel3D);
}

void MainWindowView::initMenus()
{
  this->initMenuFile();
  this->initMenuView();
  this->initMenuWorkflow();
  this->initMenuTools();
  this->initMenuPlugins();
  this->initMenuHelp();
}

void MainWindowView::initMenuFile()
{
  ui->menuFile->addAction(mActionNewProject);
  ui->menuFile->addAction(mActionOpenProject);
  mMenuRecentProjects = new QMenu(this);
  mMenuRecentProjects->addAction(mActionNotRecentProjects);
  mMenuRecentProjects->addSeparator();
  mMenuRecentProjects->addAction(mActionClearHistory);
  ui->menuFile->addMenu(mMenuRecentProjects);
  ui->menuFile->addSeparator();
  ui->menuFile->addAction(mActionSaveProject);
  ui->menuFile->addAction(mActionSaveProjectAs);
  ui->menuFile->addSeparator();
  mMenuImport = new QMenu(this);
  mMenuImport->addAction(mActionImportCameras);
  ui->menuFile->addMenu(mMenuImport);
  ui->menuFile->addSeparator();
  mMenuExport = new QMenu(this);
  //mMenuExport->addAction(mActionExportTiePoints);
  //mMenuExport->addAction(mActionExportMatches);
  //mMenuExport->addAction(mActionExportOrientations);
  //mMenuExport->addAction(mActionExportPointCloud);
  ui->menuFile->addMenu(mMenuExport);
  ui->menuFile->addSeparator();
  ui->menuFile->addAction(mActionCloseProject);
  ui->menuFile->addSeparator();
  ui->menuFile->addAction(mActionExit);
}

void MainWindowView::initMenuView()
{
  ui->menuView->addAction(mActionStartPage);
  ui->menuView->addSeparator();
  mMenuPanels = new QMenu(this);
  mMenuPanels->setIcon(QIcon(":/ico/24/img/material/24/icons8_navigation_toolbar_left_24px_1.png"));
  mMenuPanels->addAction(ui->dockWidgetProject->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetProperties->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetConsole->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetThumb->toggleViewAction());
  ui->menuView->addMenu(mMenuPanels);

  ui->menuView->addSeparator();

  mMenuToolBar = new QMenu(this);
  mMenuToolBar->setIcon(QIcon(":/ico/24/img/material/24/icons8_navigation_toolbar_top_24px_1.png"));
  mMenuToolBar->addAction(mToolBarFile->toggleViewAction());
  mMenuToolBar->addAction(mToolBarWorkflow->toggleViewAction());
  mMenuToolBar->addAction(mToolBarView->toggleViewAction());
  mMenuToolBar->addAction(mToolBar3dModel->toggleViewAction());
  mMenuToolBar->addAction(mToolBarTools->toggleViewAction());
  ui->menuView->addMenu(mMenuToolBar);
}

void MainWindowView::initMenuWorkflow()
{
}

void MainWindowView::initMenuTools()
{
  mActionCameras->setVisible(false);
  ui->menuTools->addAction(mActionCameras);
  ui->menuTools->addSeparator();
  //ui->menuTools->addAction(mActionSettings);
}

void MainWindowView::initMenuPlugins()
{

}

void MainWindowView::initMenuHelp()
{
  ui->menuHelp->addAction(mActionHelp);
  //ui->menuHelp->addSeparator();
  //ui->menuHelp->addAction(mActionAbout);
}

void MainWindowView::initProgressBar()
{
  mProgressBar = new ProgressBarWidget(this);
  mProgressBar->setVisible(false);
  mProgressBar->setMaximumSize(220, 20);
  ui->statusBar->addPermanentWidget(mProgressBar);
}

void MainWindowView::initSignalAndSlots()
{
  /* Menú Archivo */

  connect(mActionClearHistory,         &QAction::triggered, this,   &MainWindowView::clearHistory);
  connect(mActionSaveProject,          &QAction::triggered, this,   &MainWindowView::saveProject);
  connect(mActionSaveProjectAs,        &QAction::triggered, this,   &MainWindowView::saveProjectAs);
  connect(mActionExportTiePoints,      &QAction::triggered, this,   &MainWindowView::openExportFeatures);
  connect(mActionExportMatches,        &QAction::triggered, this,   &MainWindowView::openExportMatches);
  //connect(mActionExportOrientations,   &QAction::triggered, this,   &MainWindowView::openExportOrientations);
  connect(mActionExportPointCloud,     &QAction::triggered, this,   &MainWindowView::openExportPointCloud);
  connect(mActionCloseProject,         &QAction::triggered, this,   &MainWindowView::closeProject);
  connect(mActionExit, &QAction::triggered,                 this, &MainWindowView::close);

  /* Menú View */

  connect(mActionStartPage,          &QAction::triggered,   this,   &MainWindowView::openStartPage);

  /* Menú Ayuda */

  connect(mActionHelp,               SIGNAL(triggered(bool)),   this,   SIGNAL(openHelpDialog()));

  /* Panel de vistas en miniatura */

  connect(mThumbnailsWidget, &ThumbnailsWidget::openImage,                this, &MainWindowView::openImage);
  connect(mThumbnailsWidget,  SIGNAL(selectImage(size_t)),                this, SIGNAL(selectImage(size_t)));
  connect(mThumbnailsWidget,  SIGNAL(selectImages(std::vector<size_t>)),  this, SIGNAL(selectImages(std::vector<size_t>)));
  connect(mThumbnailsWidget,  SIGNAL(delete_images(std::vector<size_t>)), this, SIGNAL(delete_images(std::vector<size_t>)));
  connect(mThumbnailsWidget,  SIGNAL(imagesLoaded()),                     this, SIGNAL(imagesLoaded()));

  connect(mTreeWidgetProject, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTreeContextMenu(const QPoint &)));
  connect(mTreeWidgetProject, SIGNAL(itemSelectionChanged()),   this, SLOT(onSelectionChanged()));
  connect(mTreeWidgetProject, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));

  connect(mTabWidget, &TabWidget::allTabsClosed, this, &MainWindowView::allTabsClosed);

  connect(mActionZoomIn, &QAction::triggered, [&]() {
    if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
      graphic_viewer->zoomIn();
    }
  });

  connect(mActionZoomOut, &QAction::triggered, [&]() {
    if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
      graphic_viewer->zoomOut();
    }
  });

  connect(mActionZoomExtend, &QAction::triggered, [&]() {
    if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
      graphic_viewer->zoomExtend();
    }
  });

  connect(mActionZoom11, &QAction::triggered, [&]() {
    if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
      graphic_viewer->zoom11();
    }
  });

  connect(mActionGlobalZoom, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setGlobalZoom();
    }
  });

  connect(mActionViewFront, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setFrontView();
    }
  });

  connect(mActionViewBack, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setBackView();
    }
  });

  connect(mActionViewTop, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setTopView();
    }
  });

  connect(mActionViewBottom, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setBottomView();
    }
  });

  connect(mActionViewLeft, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setLeftView();
    }
  });

  connect(mActionViewRight, &QAction::triggered, [&]() {
    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      model3D->setRightView();
    }
  });

  connect(mActionPointMeasuse, &QAction::toggled, [&](bool checked) {

    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      const QSignalBlocker block_distance_measuse(mActionDistanceMeasuse);
      const QSignalBlocker block_angle_measuse(mActionAngleMeasure);
      mActionDistanceMeasuse->setChecked(false);
      mActionAngleMeasure->setChecked(false);

      if (checked)
        model3D->activatePicker(CCViewer3D::PickingMode::point_info);
      else
        model3D->deactivatePicker();
    }

  });

  connect(mActionDistanceMeasuse, &QAction::toggled, [&](bool checked) {

    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      const QSignalBlocker block_point_measuse(mActionPointMeasuse);
      const QSignalBlocker block_angle_measuse(mActionAngleMeasure);
      mActionPointMeasuse->setChecked(false);
      mActionAngleMeasure->setChecked(false);

      if (checked)
        model3D->activatePicker(CCViewer3D::PickingMode::distance);
      else
        model3D->deactivatePicker();
    }
  });

  connect(mActionAngleMeasure, &QAction::toggled, [&](bool checked) {

    if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
      const QSignalBlocker block_point_measuse(mActionPointMeasuse);
      const QSignalBlocker block_distance_measuse(mActionDistanceMeasuse);
      mActionPointMeasuse->setChecked(false);
      mActionDistanceMeasuse->setChecked(false);

      if (checked)
        model3D->activatePicker(CCViewer3D::PickingMode::angle);
      else
        model3D->deactivatePicker();
    }
  });

  connect(mTabWidget, &TabWidget::imageActive, [&](bool active) {
    Application &app = Application::instance();
    AppStatus *app_status = app.status();
    app_status->activeFlag(AppStatus::Flag::tab_image_active, active);
  });

  connect(mTabWidget, &TabWidget::model3dActive, [&](bool active) {
    Application &app = Application::instance();
    AppStatus *app_status = app.status();
    app_status->activeFlag(AppStatus::Flag::tab_3d_model_active, active);
  });
}

QMenu *MainWindowView::findMenu(Menu menu)
{
  QMenu *_menu = nullptr;
  switch (menu) {
    case Menu::file:
      _menu = ui->menuFile;
      break;
    case Menu::file_export:
      _menu = mMenuExport;
      break;
    case Menu::view:
      _menu = ui->menuView;
      break;
    case Menu::workflow:
      _menu = ui->menuWorkflow;
      break;
    case Menu::tools:
      _menu = ui->menuTools;
      break;
    case Menu::plugins:
      _menu = ui->menuPlugins;
      break;
    case Menu::help:
      _menu = ui->menuHelp;
      break;
  }

  return _menu;
}

QToolBar *MainWindowView::findToolbar(Toolbar toolbar)
{
  QToolBar *_toolbar = nullptr;

  switch (toolbar) {
    case Toolbar::file:
      _toolbar = mToolBarFile;
      break;
    case Toolbar::view:
      _toolbar = mToolBarView;
      break;
    case Toolbar::workflow:
      _toolbar = mToolBarWorkflow;
      break;
    case Toolbar::model3d:
      _toolbar = mToolBar3dModel;
      break;
    case Toolbar::tools:
      _toolbar = mToolBarTools;
      break;
    default:
      break;
  }

  return _toolbar;
}

void MainWindowView::update()
{

  Application &app = Application::instance();
  AppStatus *app_status = app.status();
  bool project_exists = app_status->isActive(AppStatus::Flag::project_exists);
  bool project_modified = app_status->isActive(AppStatus::Flag::project_modified);
  //bool image_open = app_status->isActive(AppStatus::Flag::image_open);
  bool processing = app_status->isActive(AppStatus::Flag::processing);
  bool image_active = app_status->isActive(AppStatus::Flag::tab_image_active);
  bool model_3d_active = app_status->isActive(AppStatus::Flag::tab_3d_model_active);

  mActionZoomIn->setEnabled(image_active);
  mActionZoomOut->setEnabled(image_active);
  mActionZoomExtend->setEnabled(image_active);
  mActionZoom11->setEnabled(image_active);

  mActionGlobalZoom->setEnabled(model_3d_active);
  mActionViewFront->setEnabled(model_3d_active);
  mActionViewTop->setEnabled(model_3d_active);
  mActionViewLeft->setEnabled(model_3d_active);
  mActionViewRight->setEnabled(model_3d_active);
  mActionViewBack->setEnabled(model_3d_active);
  mActionViewBottom->setEnabled(model_3d_active);
  mActionPointMeasuse->setEnabled(model_3d_active);
  mActionDistanceMeasuse->setEnabled(model_3d_active);
  mActionAngleMeasure->setEnabled(model_3d_active);

  mMenuRecentProjects->setEnabled(!processing);
  mActionNotRecentProjects->setVisible(mHistory.size() == 0);
  mActionClearHistory->setEnabled(mHistory.size() > 0);
  mActionSaveProject->setEnabled(project_exists && project_modified && !processing);
  mActionSaveProjectAs->setEnabled(project_exists && !processing);
  mActionCloseProject->setEnabled(project_exists && !processing);
  mActionExit->setEnabled(!processing);
}

void MainWindowView::retranslate()
{
  mActionNewProject->setText(QApplication::translate("MainWindowView", "New Project", nullptr));
  mActionOpenProject->setText(QApplication::translate("MainWindowView", "Open Project", nullptr));
  mActionSaveProject->setText(QApplication::translate("MainWindowView", "Save Project", nullptr));
  mActionSaveProjectAs->setText(QApplication::translate("MainWindowView", "Save Project As...", nullptr));
  mActionImportCameras->setText(QApplication::translate("MainWindowView", "Import Cameras", nullptr));
  mActionExportTiePoints->setText(QApplication::translate("MainWindowView", "Export tie points", nullptr));
  mActionExportMatches->setText(QApplication::translate("MainWindowView", "Export Matches", nullptr));
  //mActionExportOrientations->setText(QApplication::translate("MainWindowView", "Export Orientations", nullptr));
  mActionExportPointCloud->setText(QApplication::translate("MainWindowView", "Export Point Cloud", nullptr));
  mActionCloseProject->setText(QApplication::translate("MainWindowView", "Close Project", nullptr));
  mActionExit->setText(QApplication::translate("MainWindowView", "Exit", nullptr));
  mActionStartPage->setText(QApplication::translate("MainWindowView", "Start Page", nullptr));
  mActionCameras->setText(QApplication::translate("MainWindowView", "Cameras", nullptr));
  mActionHelp->setText(QApplication::translate("MainWindowView", "Help", nullptr));
  //mActionAbout->setText(QApplication::translate("MainWindowView", "About", nullptr));
  //mActionSettings->setText(QApplication::translate("MainWindowView", "Settings", nullptr));
  mActionNotRecentProjects->setText(QApplication::translate("MainWindowView", "Not recent projects", nullptr));
  mActionClearHistory->setText(QApplication::translate("MainWindowView", "Clear History", nullptr));
  mActionOpenImage->setText(QApplication::translate("MainWindowView", "Open Image", nullptr));
  mActionDeleteImage->setText(QApplication::translate("MainWindowView", "Delete Image", nullptr));
  mActionViewKeypoints->setText(QApplication::translate("MainWindowView", "View Keypoints", nullptr));
  mActionViewMatches->setText(QApplication::translate("MainWindowView", "View Matches", nullptr));
  mActionOpenModel3D->setText(QApplication::translate("MainWindowView", "Open Point Cloud", nullptr));
  mActionZoomIn->setText(QApplication::translate("MainWindowView", "Zoom In"));
  mActionZoomOut->setText(QApplication::translate("MainWindowView", "Zoom Out"));
  mActionZoomExtend->setText(QApplication::translate("MainWindowView", "Zoom Extend"));
  mActionZoom11->setText(QApplication::translate("MainWindowView", "Zoom 1:1"));
  mActionGlobalZoom->setText(QApplication::translate("MainWindowView", "Global Zoom", nullptr));
  mActionViewFront->setText(QApplication::translate("MainWindowView", "View Front", nullptr));
  mActionViewTop->setText(QApplication::translate("MainWindowView", "View Top", nullptr));
  mActionViewLeft->setText(QApplication::translate("MainWindowView", "View Left", nullptr));
  mActionViewRight->setText(QApplication::translate("MainWindowView", "View Right", nullptr));
  mActionViewBack->setText(QApplication::translate("MainWindowView", "View Back", nullptr));
  mActionViewBottom->setText(QApplication::translate("MainWindowView", "View Bottom", nullptr));
  mActionPointMeasuse->setText(QApplication::translate("MainWindowView", "Point Measure", nullptr));
  mActionDistanceMeasuse->setText(QApplication::translate("MainWindowView", "Distance Measure", nullptr));
  mActionAngleMeasure->setText(QApplication::translate("MainWindowView", "Angle Measure", nullptr));


  mToolBarFile->setWindowTitle(QCoreApplication::translate("MainWindowView", "File", nullptr));
  mToolBarWorkflow->setWindowTitle(QCoreApplication::translate("MainWindowView", "Workflow", nullptr));
  mToolBarView->setWindowTitle(QCoreApplication::translate("MainWindowView", "Image viewer tools", nullptr));
  mToolBar3dModel->setWindowTitle(QCoreApplication::translate("MainWindowView", "3D Viewer", nullptr));
  mToolBarTools->setWindowTitle(QCoreApplication::translate("MainWindowView", "Tools", nullptr));

  mMenuRecentProjects->setTitle(QApplication::translate("MainWindowView", "Recent Projects", nullptr));
  mMenuImport->setTitle(QApplication::translate("MainWindowView", "Import", nullptr));
  mMenuExport->setTitle(QApplication::translate("MainWindowView", "Export", nullptr));
  mMenuPanels->setTitle(QApplication::translate("MainWindowView", "Dockable panels", nullptr));
  mMenuToolBar->setTitle(QApplication::translate("MainWindowView", "Toolbars", nullptr));

#ifndef QT_NO_SHORTCUT
  mActionNewProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+N", nullptr));
  mActionOpenProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+O", nullptr));
  mActionSaveProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+S", nullptr));
  mActionSaveProjectAs->setShortcut(QApplication::translate("MainWindowView", "Ctrl+Shift+S", nullptr));
  mActionExit->setShortcut(QApplication::translate("MainWindowView", "Ctrl+F4", nullptr));
  mActionHelp->setShortcut(QApplication::translate("MainWindowView", "F1", nullptr));
#endif // QT_NO_SHORTCUT
}

void MainWindowView::closeEvent(QCloseEvent *event)
{
  emit exit();

  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
}

} // namespace graphos
