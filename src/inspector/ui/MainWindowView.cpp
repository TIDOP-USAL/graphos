#include "MainWindowView.h"
#include "ui_MainWindowView.h"

#include "inspector/widgets/ThumbnailsWidget.h"
#include "inspector/widgets/LogWidget.h"
#include "inspector/ui/utils/TabHandler.h"

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

namespace inspector
{

namespace ui
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
  dense_model
};

MainWindowView::MainWindowView(QWidget *parent)
  : QMainWindow(parent),
    mActionNewProject(new QAction(this)),
    mActionOpenProject(new QAction(this)),
    mActionSaveProject(new QAction(this)),
    mActionSaveProjectAs(new QAction(this)),
    mActionCloseProject(new QAction(this)),
    mActionExit(new QAction(this)),
    mActionStartPage(new QAction(this)),
    mActionLoadImages(new QAction(this)),
    mActionFeatureExtraction(new QAction(this)),
    mActionFeatureMatching(new QAction(this)),
    mActionOrientation(new QAction(this)),
    mActionDensification(new QAction(this)),
    mActionCameras(new QAction(this)),
    mActionGeoreference(new QAction(this)),
    mActionSettings(new QAction(this)),
    mActionHelp(new QAction(this)),
    mActionAbout(new QAction(this)),
    mActionImportCameras(new QAction(this)),
    mActionExportTiePoints(new QAction(this)),
    mActionExportMatches(new QAction(this)),
    mActionExportOrientations(new QAction(this)),
    mActionExportPointCloud(new QAction(this)),
    mActionFeaturesViewer(new QAction(this)),
    mActionMatchesViewer(new QAction(this)),
    mActionDtm(new QAction(this)),
    mActionOrtho(new QAction(this)),
    //mActionPassPointsViewer(new QAction(this)),
    mActionNotRecentProjects(new QAction(this)),
    mActionClearHistory(new QAction(this)),
    mActionOpenImage(new QAction(this)),
    mActionDeleteImage(new QAction(this)),
    mActionViewKeypoints(new QAction(this)),
    mActionViewMatches(new QAction(this)),
    ui(new Ui::MainWindowView)
{
  ui->setupUi(this);

  this->initUI();
  this->initSignalAndSlots();
}

MainWindowView::~MainWindowView()
{
  delete ui;
}

void MainWindowView::addActionToMenu(QAction *action, Menu menu)
{
  if (QMenu *_menu = findMenu(menu)) {
    _menu->addAction(action);
    this->update();
  }
}

void MainWindowView::addSeparatorToMenu(Menu menu)
{
  if (QMenu *_menu = findMenu(menu)) {
    _menu->addSeparator();
    this->update();
  }
}

void MainWindowView::addActionToToolbar(QAction *action, Toolbar toolbar)
{
  if (QToolBar *_toolbar = findToolbar(toolbar)) {
    _toolbar->addAction(action);
    this->update();
  }
}

void MainWindowView::addSeparatorToToolbar(Toolbar toolbar)
{
  if (QToolBar *_toolbar = findToolbar(toolbar)) {
    _toolbar->addSeparator();
    this->update();
  }
}

void MainWindowView::clear()
{
  setWindowTitle(QString("Inspector Image"));
  const QSignalBlocker blockerTreeWidgetProject(mTreeWidgetProject);
  mTreeWidgetProject->clear();
  const QSignalBlocker blockerThumbnailsWidget(mThumbnailsWidget);
  mThumbnailsWidget->clear();
  ui->treeWidgetProperties->clear();
  mFlags.clear();

  if (mTabHandler) mTabHandler->clear();

  update();
}

void MainWindowView::setProjectTitle(const QString &title)
{
  this->setWindowTitle(QString("Inspector Image - ").append(title));

  QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0);
  if (itemProject == nullptr) {
    itemProject = new QTreeWidgetItem();
    mTreeWidgetProject->addTopLevelItem(itemProject);
    itemProject->setExpanded(true);
    itemProject->setData(0, Qt::UserRole, inspector::ui::project);
  }

  itemProject->setText(0, tr("Project: ").append(title));
}

void MainWindowView::setFlag(MainWindowView::Flag flag, bool value)
{
  mFlags.activeFlag(flag, value);
  update();
}

void MainWindowView::addImage(const QString &image)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    /* Fotogramas */

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages == nullptr) {
      itemImages = new QTreeWidgetItem();
      itemImages->setText(0, tr("Images"));
      itemImages->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
      itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
      itemImages->setData(0, Qt::UserRole, inspector::ui::images);
      itemProject->addChild(itemImages);
      itemImages->setExpanded(true);
    }

    /* Se añade el fotograma al árbol del proyecto */
    QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
    itemPhotogram->setText(0, QFileInfo(image).baseName());
    itemPhotogram->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-image-file.png"));
    itemPhotogram->setToolTip(0, image);
    itemPhotogram->setData(0, Qt::UserRole, inspector::ui::image);
    itemImages->addChild(itemPhotogram);
    itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));

    update();
  }

  mThumbnailsWidget->addThumbnail(image);
}

void MainWindowView::addImages(const QStringList &images)
{
  if (images.empty() == false) {
    for (auto &image : images) {
      if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        /* Fotogramas */

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
          QTreeWidgetItem *temp = itemProject->child(i);
          //if (temp->text(0).compare(tr("Images")) == 0) {
          if (temp->data(0, Qt::UserRole) == inspector::ui::images){
            itemImages = itemProject->child(i);
            break;
          }
        }

        if (itemImages == nullptr) {
          itemImages = new QTreeWidgetItem();
          itemImages->setText(0, tr("Images"));
          itemImages->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
          itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
          itemImages->setData(0, Qt::UserRole, inspector::ui::images);
          itemProject->addChild(itemImages);
          itemImages->setExpanded(true);
        }

        /* Se añade el fotograma al árbol del proyecto */
        QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
        itemPhotogram->setText(0, QFileInfo(image).baseName());
        itemPhotogram->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-image-file.png"));
        itemPhotogram->setToolTip(0, image);
        itemPhotogram->setData(0, Qt::UserRole, inspector::ui::image);
        itemImages->addChild(itemPhotogram);
        itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));
        
        update();
      }
    }

    mThumbnailsWidget->addThumbnails(images);

  }
}

void MainWindowView::setActiveImage(const QString &image)
{
  const QSignalBlocker blocker1(mTreeWidgetProject);
  const QSignalBlocker blocker2(mThumbnailsWidget);

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        itemImages->child(i)->setSelected(temp->text(0).compare(image) == 0);
      }
    }
  }

  mThumbnailsWidget->setActiveImage(image);

}

void MainWindowView::setActiveImages(const QStringList &images)
{
  const QSignalBlocker blocker1(mTreeWidgetProject);
  const QSignalBlocker blocker2(mThumbnailsWidget);

  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        temp->setSelected(false);
        for (auto &image : images){
          if (temp->text(0).compare(image) == 0){
            temp->setSelected(true);
            break;
          }
        }
      }
    }
  }

  mThumbnailsWidget->setActiveImages(images);
}

void MainWindowView::addFeatures(const QString &features)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->text(0).compare(features) == 0){
          itemImage = itemImages->child(i);
          break;
        }
      }
      if (itemImage)
        itemImage->setData(0, Qt::UserRole, inspector::ui::image_features);
    }
  }
}

void MainWindowView::addMatches(const QString &left, const QString &right)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {
      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->text(0).compare(left) == 0){
          itemImage = itemImages->child(i);
          break;
        }
      }

      itemImage->setData(0, Qt::UserRole, inspector::ui::image_features_matches);
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
      //itemModels->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
      //itemModels->setFlags(itemImages->flags() | Qt::ItemIsTristate);
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemSparseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("SparseModel")) == 0) {
        itemSparseModel = temp;
        break;
      }
    }

    if (itemSparseModel == nullptr){
      itemSparseModel = new QTreeWidgetItem();
      itemModels->addChild(itemSparseModel);
    }

    itemSparseModel->setText(0, "SparseModel");
    itemSparseModel->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
    itemSparseModel->setToolTip(0, sparseModel);
    itemSparseModel->setData(0, Qt::UserRole, inspector::ui::sparse_model);
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
      //itemModels->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-pictures-folder.png"));
      //itemModels->setFlags(itemImages->flags() | Qt::ItemIsTristate);
      itemProject->addChild(itemModels);
      itemModels->setExpanded(true);
    }

    QTreeWidgetItem *itemDenseModel = nullptr;
    for (int i = 0; i < itemModels->childCount(); i++) {
      QTreeWidgetItem *temp = itemModels->child(i);
      if (temp->text(0).compare(tr("DenseModel")) == 0) {
        itemDenseModel = temp;
        break;
      }
    }

    if (itemDenseModel == nullptr){
      itemDenseModel = new QTreeWidgetItem();
      itemModels->addChild(itemDenseModel);
    }

    itemDenseModel->setText(0, "DenseModel");
    itemDenseModel->setIcon(0, QIcon(":/ico/48/img/material/48/icons8-3d-model.png"));
    itemDenseModel->setToolTip(0, denseModel);
    itemDenseModel->setData(0, Qt::UserRole, inspector::ui::dense_model);
  }
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

QProgressBar *MainWindowView::progressBar()
{
  return mProgressBar;
}

TabHandler *MainWindowView::tabHandler()
{
  return mTabHandler;
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

void MainWindowView::deleteImage(const QString &file)
{
  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

    QTreeWidgetItem *itemImages = nullptr;
    for (int i = 0; i < itemProject->childCount(); i++) {
      QTreeWidgetItem *temp = itemProject->child(i);
      //if (temp->text(0).compare(tr("Images")) == 0) {
      if (temp->data(0, Qt::UserRole) == inspector::ui::images){
        itemImages = itemProject->child(i);
        break;
      }
    }

    if (itemImages) {

      // Se busca la imagen en el árbol
      QTreeWidgetItem *itemImage = nullptr;
      for (int i = 0; i < itemImages->childCount(); i++) {
        QTreeWidgetItem *temp = itemImages->child(i);
        if (temp->text(0).compare(file) == 0) {
          itemImage = itemImages->child(i);
          delete itemImage;
          itemImage = nullptr;
          break;
        }
      }
      itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));
    }

    setFlag(MainWindowView::Flag::images_added, itemProject->childCount() > 0);
  }

  mThumbnailsWidget->deleteThumbnail(file);
}

void MainWindowView::deleteFeatures(const QString &session, const QString &feat)
{

//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    QTreeWidgetItem *itemSessions = nullptr;
//    for (int i = 0; i < itemProject->childCount(); i++) {
//      if (QTreeWidgetItem *temp = itemProject->child(i)){
//        if (temp->text(0).compare(tr("Sessions")) == 0) {
//          itemSessions = itemProject->child(i);
//          break;
//        }
//      }
//    }

//    if (itemSessions != nullptr) {

//      QTreeWidgetItem *itemFeatures = nullptr;
//      for (int i = 0; i < itemSessions->childCount(); i++) {
//        if (QTreeWidgetItem *itemSession = itemSessions->child(i)){
//          if (itemSession->text(0).compare(session) == 0){

//            for (int j = 0; j < itemSession->childCount(); j++) {
//              if (QTreeWidgetItem *temp = itemSession->child(j)){
//                if (temp->text(0).compare(tr("Features")) == 0) {
//                  itemFeatures = itemSession->child(j);
//                  break;
//                }
//              }
//            }

//            break;

//          }
//        }
//      }

//      if (itemFeatures != nullptr) {

        
//        QTreeWidgetItem *itemImagesFeatures = nullptr;
//        for (int i = 0; i < itemFeatures->childCount(); i++) {
//          QTreeWidgetItem *temp = itemFeatures->child(i);
//          if (temp && temp->text(0).compare(tr("Keypoints")) == 0) {
//            itemImagesFeatures = itemFeatures->child(i);
//            break;
//          }
//        }

//        if (itemImagesFeatures != nullptr) {

//          QTreeWidgetItem *itemImageFeatures = nullptr;
//          for (int i = 0; i < itemImagesFeatures->childCount(); i++) {
//            QTreeWidgetItem *temp = itemImagesFeatures->child(i);
//            if (temp->toolTip(0).compare(feat) == 0) {
//              itemImageFeatures = itemImagesFeatures->child(i);
//              delete itemImageFeatures;
//              itemImageFeatures = nullptr;
//              break;
//            }
//          }

//        }

//      }
    
//    }

//  }
}

void MainWindowView::deleteMatches(const QString &session, const QString &matches)
{

//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    QTreeWidgetItem *itemSessions = nullptr;
//    for (int i = 0; i < itemProject->childCount(); i++) {
//      if (QTreeWidgetItem *temp = itemProject->child(i)){
//        if (temp->text(0).compare(tr("Sessions")) == 0) {
//          itemSessions = itemProject->child(i);
//          break;
//        }
//      }
//    }

//    if (itemSessions != nullptr) {

//      QTreeWidgetItem *itemMatches = nullptr;
//      for (int i = 0; i < itemSessions->childCount(); i++) {
//        if (QTreeWidgetItem *itemSession = itemSessions->child(i)){
//          if (itemSession->text(0).compare(session) == 0){

//            for (int j = 0; j < itemSession->childCount(); j++) {
//              if (QTreeWidgetItem *temp = itemSession->child(j)){
//                if (temp->text(0).compare(tr("Matches")) == 0) {
//                  itemMatches = itemSession->child(j);
//                  break;
//                }
//              }
//            }

//            break;

//          }
//        }
//      }

//      if (itemMatches != nullptr) {


//        /* Image pairs */

//        QTreeWidgetItem *itemImagePairs = nullptr;
//        for (int i = 0; i < itemMatches->childCount(); i++) {
//          QTreeWidgetItem *temp = itemMatches->child(i);
//          if (temp && temp->text(0).compare(tr("Image Pairs")) == 0) {
//            itemImagePairs = itemMatches->child(i);
//            break;
//          }
//        }

//        if (itemImagePairs != nullptr) {
//          QTreeWidgetItem *itemLeftImage = nullptr;
//          for (int i = 0; i < itemImagePairs->childCount(); i++) {
//            itemLeftImage = itemImagePairs->child(i);
//            if (itemLeftImage != nullptr) {

//              QTreeWidgetItem *itemRightImage = nullptr;
//              for (int j = 0; j < itemLeftImage->childCount(); j++) {
//                QTreeWidgetItem *temp = itemLeftImage->child(j);
//                if (temp && temp->toolTip(0).compare(matches) == 0) {
//                  itemRightImage = itemLeftImage->child(j);
//                  break;
//                }
//              }

//              if (itemRightImage != nullptr) {
//                delete itemRightImage;
//                itemRightImage = nullptr;
//                if (itemLeftImage->childCount() == 0){
//                  delete itemLeftImage;
//                  itemLeftImage = nullptr;
//                }
//                break;
//              }
//            }

//          }

//        }

//      }

//    }

//  }
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
  if (item[0]->data(0, Qt::UserRole) == inspector::ui::project){

  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::images){
  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::image ||
             item[0]->data(0, Qt::UserRole) == inspector::ui::image_features ||
             item[0]->data(0, Qt::UserRole) == inspector::ui::image_features_matches){
    int size = item.size();
    if(size > 0){
      if (size == 1) {
        emit selectImage(item[0]->text(0));
      } else {
        QStringList selected_images;
        for (int i = 0; i < size; i++){
          selected_images.push_back(item[i]->text(0));
        }
        emit selectImages(selected_images);
      }
    }
  }
}

void MainWindowView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
  if (item){
    if (item->data(0, Qt::UserRole) == inspector::ui::image ||
        item->data(0, Qt::UserRole) == inspector::ui::image_features ||
        item->data(0, Qt::UserRole) == inspector::ui::image_features_matches){
     emit openImage(item->text(column));
   } else if (item->data(0, Qt::UserRole) == inspector::ui::sparse_model) {
     emit openModel3D(item->toolTip(column), true);
   } else if (item->data(0, Qt::UserRole) == inspector::ui::dense_model) {
     emit openModel3D(item->toolTip(column), false);
   }
   /*else if (item->data(0, Qt::UserRole) == inspector::ui::pair_right){
     QString session = item->parent()->parent()->parent()->parent()->text(0);
     emit openImageMatches(session, item->parent()->text(0), item->text(column));
   } else if (item->data(0, Qt::UserRole) == inspector::ui::features_image){
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

  if (item->data(0, Qt::UserRole) == inspector::ui::project){
    /*QAction *selectedItem = */mMenuTreeProjectImages->exec(globalPos);
  } else if (item->data(0, Qt::UserRole) == inspector::ui::images){

  } else if (item->data(0, Qt::UserRole) == inspector::ui::image ||
             item->data(0, Qt::UserRole) == inspector::ui::image_features ||
             item->data(0, Qt::UserRole) == inspector::ui::image_features_matches){

    if (item->data(0, Qt::UserRole) == inspector::ui::image) {
      mActionViewKeypoints->setEnabled(false);
      mActionViewMatches->setEnabled(false);
    } else if (item->data(0, Qt::UserRole) == inspector::ui::image_features) {
      mActionViewKeypoints->setEnabled(true);
      mActionViewMatches->setEnabled(false);
    } else {
      mActionViewKeypoints->setEnabled(true);
      mActionViewMatches->setEnabled(true);
    }

    if (QAction *selectedItem = mMenuTreeProjectImage->exec(globalPos)) {
      if (selectedItem->text() == tr("Open Image")) {
        emit openImage(item->text(0));
      } else if (selectedItem->text() == tr("Delete Image")) {
        emit deleteImages(QStringList(item->text(0)));
      } else if (selectedItem->text() == tr("View Keypoints")) {
        emit openKeypointsViewerFromImage(item->text(0));
      } else if (selectedItem->text() == tr("View Matches")) {
        emit openMatchesViewer();
      }
    }
  } /*else if (item->data(0, Qt::UserRole) == inspector::ui::detector){

  } else if (item->data(0, Qt::UserRole) == inspector::ui::descriptor){

  } else if (item->data(0, Qt::UserRole) == inspector::ui::pair_left){
    QMenu menu;
    menu.addAction(tr("View Matches"));
    if (QAction *selectedItem = menu.exec(globalPos)) {
      if (selectedItem->text() == tr("View Matches")) {
        QString session = item->parent()->parent()->parent()->text(0);
        emit openMatchesViewer(session, QFileInfo(item->text(0)).baseName(), QString());
      }
    }
  } else if (item->data(0, Qt::UserRole) == inspector::ui::pair_right){
    QMenu menu;
    menu.addAction(tr("View Matches"));
    if (QAction *selectedItem = menu.exec(globalPos)) {
      if (selectedItem->text() == tr("View Matches")) {
        QString session = item->parent()->parent()->parent()->parent()->text(0);
        emit openMatchesViewer(session, QFileInfo(item->parent()->text(0)).baseName(), QFileInfo(item->text(0)).baseName());
      }
    }
  }*/
}

void MainWindowView::initUI()
{
  setWindowTitle(QString("Inspector Image"));

  mLayoutCentral = new QGridLayout(this->centralWidget());
  mLayoutCentral->setSpacing(6);
  mLayoutCentral->setContentsMargins(0,0,0,0);

  this->initTabHandler();
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

void MainWindowView::initTabHandler()
{
  mTabHandler = new TabHandler(this->centralWidget());
  mLayoutCentral->addWidget(mTabHandler);
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

  QIcon iconLoadImages;
  iconLoadImages.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-add-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionLoadImages->setIcon(iconLoadImages);

  QIcon iconFeatureExtraction;
  iconFeatureExtraction.addFile(QStringLiteral(":/ico/24/img/material/24/features.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionFeatureExtraction->setIcon(iconFeatureExtraction);

  QIcon iconFeatureMatching;
  iconFeatureMatching.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-match_view"), QSize(), QIcon::Normal, QIcon::Off);
  mActionFeatureMatching->setIcon(iconFeatureMatching);

//  QIcon iconOrientation;
//  iconOrientation.addFile(QStringLiteral(":/ico/24/img/material/24/match_view.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionOrientation->setIcon(iconOrientation);

//  QIcon iconDensification;
//  iconDensification.addFile(QStringLiteral(":/ico/24/img/material/24/match_view.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionDensification->setIcon(iconDensification);

  QIcon iconCameras;
  iconCameras.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-camera-outline-2.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionCameras->setIcon(iconCameras);

  QIcon iconGeoreference;
  iconGeoreference.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-coordinate-system.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionGeoreference->setIcon(iconGeoreference);

  QIcon iconFeaturesViewer;
  iconFeaturesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_points_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionFeaturesViewer->setIcon(iconFeaturesViewer);

  QIcon iconMatchesViewer;
  iconMatchesViewer.addFile(QStringLiteral(":/ico/24/img/material/24/view_match_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionMatchesViewer->setIcon(iconMatchesViewer);

  QIcon iconSettings;
  iconSettings.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-settings.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionSettings->setIcon(iconSettings);

  QIcon iconHelp;
  iconHelp.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_help_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionHelp->setIcon(iconHelp);

  QIcon iconAbout;
  iconAbout.addFile(QStringLiteral(":/ico/24/img/material/24/icons8_about_24px.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionAbout->setIcon(iconAbout);

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

  mActionViewKeypoints->setIcon(iconFeaturesViewer);

  mActionViewMatches->setIcon(iconMatchesViewer);

  //mActionDtm->setIcon(iconMatchesViewer);
  //mActionOrtho->setIcon(iconMatchesViewer);
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
  mToolBarWorkflow->addAction(mActionLoadImages);
  mToolBarWorkflow->addSeparator();
  mToolBarWorkflow->addAction(mActionFeatureExtraction);
  mToolBarWorkflow->addAction(mActionFeatureMatching);
  this->addToolBar(Qt::TopToolBarArea, mToolBarWorkflow);
}

void MainWindowView::initToolbarView()
{
  mToolBarView = new QToolBar(this);
  mToolBarView->setObjectName("ToolBarView");
  mToolBarView->addAction(mTabHandler->actionZoomIn());
  mToolBarView->addAction(mTabHandler->actionZoomOut());
  mToolBarView->addAction(mTabHandler->actionZoom11());
  mToolBarView->addAction(mTabHandler->actionZoomExtend());
  this->addToolBar(Qt::TopToolBarArea, mToolBarView);
}

void MainWindowView::initToolbar3dModel()
{
  mToolBar3dModel = new QToolBar(this);
  mToolBar3dModel->setObjectName("ToolBar3dModel");
  mToolBar3dModel->addAction(mTabHandler->actionGlobalZoom());
  mToolBar3dModel->addSeparator();
  mToolBar3dModel->addAction(mTabHandler->actionViewFront());
  mToolBar3dModel->addAction(mTabHandler->actionViewBack());
  mToolBar3dModel->addAction(mTabHandler->actionViewTop());
  mToolBar3dModel->addAction(mTabHandler->actionViewBottom());
  mToolBar3dModel->addAction(mTabHandler->actionViewLeft());
  mToolBar3dModel->addAction(mTabHandler->actionViewRight());
  this->addToolBar(Qt::TopToolBarArea, mToolBar3dModel);
}

void MainWindowView::initToolbarTools()
{
  mToolBarTools = new QToolBar(this);
  mToolBarTools->setObjectName("ToolBarTools");
  mToolBarTools->addAction(mActionCameras);
  mToolBarTools->addSeparator();
  mToolBarTools->addAction(mActionGeoreference);
  mToolBarTools->addSeparator();
  mToolBarTools->addAction(mActionFeaturesViewer);
  mToolBarTools->addAction(mActionMatchesViewer);
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
  mMenuTreeProjectImages->addAction(mActionLoadImages);
  mMenuTreeProjectImage = new QMenu(this);
  mMenuTreeProjectImage->addAction(mActionOpenImage);
  mMenuTreeProjectImage->addAction(mActionDeleteImage);
  mMenuTreeProjectImage->addSeparator();
  mMenuTreeProjectImage->addAction(mActionViewKeypoints);
  mMenuTreeProjectImage->addSeparator();
  mMenuTreeProjectImage->addAction(mActionViewMatches);
}

void MainWindowView::initMenus()
{
  this->initMenuFile();
  this->initMenuView();
  this->initMenuWorkflow();
  this->initMenuTools();
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
  mMenuExport->addAction(mActionExportTiePoints);
  mMenuExport->addAction(mActionExportMatches);
  mMenuExport->addAction(mActionExportOrientations);
  mMenuExport->addAction(mActionExportPointCloud);
  //mMenuExportTiePoints = new QMenu(tr("Tie Points"), this);
  //mMenuExportMatches = new QMenu(tr("Matches"), this);
  //mMenuExportTiePoints->addAction(mActionExportTiePointsCvXml);
  //mMenuExportTiePoints->addAction(mActionExportTiePointsCvYml);
  //mMenuExportMatches->addAction(mActionExportMatchesToCvXml);
  //mMenuExportMatches->addAction(mActionExportMatchesToCvYml);
  //mMenuExportMatches->addAction(mActionExportMatchesToTxt);
  //mMenuExport->addMenu(mMenuExportTiePoints);
  //mMenuExport->addMenu(mMenuExportMatches);
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
  ui->menuWorkflow->addAction(mActionLoadImages);
  ui->menuWorkflow->addSeparator();
  ui->menuWorkflow->addAction(mActionFeatureExtraction);
  ui->menuWorkflow->addAction(mActionFeatureMatching);
  ui->menuWorkflow->addAction(mActionOrientation);
  ui->menuWorkflow->addAction(mActionDensification);
}

void MainWindowView::initMenuTools()
{
  ui->menuTools->addAction(mActionCameras);
  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionGeoreference);
  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionDtm);
  //ui->menuTools->addAction(mActionOrtho);
  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionFeaturesViewer);
  ui->menuTools->addAction(mActionMatchesViewer);
  //ui->menuTools->addAction(mActionPassPointsViewer);
  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionSettings);

}

void MainWindowView::initMenuHelp()
{
  ui->menuHelp->addAction(mActionHelp);
  ui->menuHelp->addSeparator();
  ui->menuHelp->addAction(mActionAbout);
}

void MainWindowView::initProgressBar()
{
  mProgressBar = new QProgressBar(this);
  mProgressBar->setVisible(false);
  mProgressBar->setMaximumSize(150, 20);
  ui->statusBar->addPermanentWidget(mProgressBar);
}

void MainWindowView::initSignalAndSlots()
{
  /* Menú Archivo */

  connect(mActionNewProject,           &QAction::triggered, this,   &MainWindowView::openNew);
  connect(mActionOpenProject,          &QAction::triggered, this,   &MainWindowView::openProject);
  connect(mActionClearHistory,         &QAction::triggered, this,   &MainWindowView::clearHistory);
  connect(mActionSaveProject,          &QAction::triggered, this,   &MainWindowView::saveProject);
  connect(mActionSaveProjectAs,        &QAction::triggered, this,   &MainWindowView::saveProjectAs);
  connect(mActionImportCameras,        &QAction::triggered, this,   &MainWindowView::openCamerasImport);
  connect(mActionExportTiePoints,      &QAction::triggered, this,   &MainWindowView::openExportFeatures);
  connect(mActionExportMatches,        &QAction::triggered, this,   &MainWindowView::openExportMatches);
  connect(mActionExportOrientations,   &QAction::triggered, this,   &MainWindowView::openExportOrientations);
  connect(mActionExportPointCloud,     &QAction::triggered, this,   &MainWindowView::openExportPointCloud);
  connect(mActionCloseProject,         &QAction::triggered, this,   &MainWindowView::closeProject);
  connect(mActionExit,                 &QAction::triggered, this,   &MainWindowView::exit);

  /* Menú View */

  connect(mActionStartPage,          &QAction::triggered,   this,   &MainWindowView::openStartPage);

  /* Menú Flujo de trabajo */

  connect(mActionLoadImages,         &QAction::triggered,   this,   &MainWindowView::loadImages);
  connect(mActionFeatureExtraction,  &QAction::triggered,   this,   &MainWindowView::openFeatureExtraction);
  connect(mActionFeatureMatching,    &QAction::triggered,   this,   &MainWindowView::openFeatureMatching);
  connect(mActionOrientation,        &QAction::triggered,   this,   &MainWindowView::openOrientation);
  connect(mActionDensification,      &QAction::triggered,   this,   &MainWindowView::openDensification);

  /* Menú herramientas */

  connect(mActionCameras,            &QAction::triggered,   this,   &MainWindowView::openCamerasDialog);
  connect(mActionGeoreference,       &QAction::triggered,   this,   &MainWindowView::openGeoreferenceDialog);
  connect(mActionDtm,                &QAction::triggered,   this,   &MainWindowView::openDtmDialog);
  connect(mActionFeaturesViewer,     SIGNAL(triggered(bool)),   this,   SIGNAL(openKeypointsViewer()));
  connect(mActionMatchesViewer,      SIGNAL(triggered(bool)),   this,   SIGNAL(openMatchesViewer()));
  //connect(mActionPassPointsViewer,   SIGNAL(triggered(bool)),   this,   SIGNAL(openMultiviewMatchingAssessment()));
  connect(mActionSettings,           SIGNAL(triggered(bool)),   this,   SIGNAL(openSettings()));

  /* Menú Ayuda */

  connect(mActionHelp,               SIGNAL(triggered(bool)),   this,   SIGNAL(openHelpDialog()));
  connect(mActionAbout,              SIGNAL(triggered(bool)),   this,   SIGNAL(openAboutDialog()));

  /* Panel de vistas en miniatura */

  connect(mThumbnailsWidget,  SIGNAL(openImage(QString)),        this, SIGNAL(openImage(QString)));
  connect(mThumbnailsWidget,  SIGNAL(selectImage(QString)),      this, SIGNAL(selectImage(QString)));
  connect(mThumbnailsWidget,  SIGNAL(selectImages(QStringList)), this, SIGNAL(selectImages(QStringList)));
  connect(mThumbnailsWidget,  SIGNAL(deleteImages(QStringList)), this, SIGNAL(deleteImages(QStringList)));
  connect(mThumbnailsWidget,  SIGNAL(imagesLoaded()),            this, SIGNAL(imagesLoaded()));

  connect(mTreeWidgetProject, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onTreeContextMenu(const QPoint &)));
  connect(mTreeWidgetProject, SIGNAL(itemSelectionChanged()),   this, SLOT(onSelectionChanged()));
  connect(mTreeWidgetProject, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(onItemDoubleClicked(QTreeWidgetItem *, int)));

}

QMenu *MainWindowView::findMenu(Menu menu)
{
  QMenu *_menu = nullptr;
  switch (menu) {
    case Menu::file:
      _menu = ui->menuFile;
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
  bool bProjectExists = mFlags.isActive(Flag::project_exists);
  bool bProjectModified = mFlags.isActive(Flag::project_modified);
  bool bImageOpen = mFlags.isActive(Flag::image_open);
  bool bProcessing = mFlags.isActive(Flag::processing);
  bool bLoadingImages = mFlags.isActive(Flag::loading_images);
  bool bImagesLoaded = mFlags.isActive(Flag::images_added);
  bool bFeatureExtraction = mFlags.isActive(Flag::feature_extraction);
  bool bFeatureMatching = mFlags.isActive(Flag::feature_matching);
  bool bOriented = mFlags.isActive(Flag::oriented);
  bool bAbsoluteOriented = mFlags.isActive(Flag::absolute_oriented);
  bool bDenseModel = mFlags.isActive(Flag::dense_model);

  mActionNewProject->setEnabled(!bProcessing);
  mActionOpenProject->setEnabled(!bProcessing);
  mMenuRecentProjects->setEnabled(!bProcessing);
  mActionNotRecentProjects->setVisible(mHistory.size() == 0);
  mActionClearHistory->setEnabled(mHistory.size() > 0);
  mActionImportCameras->setEnabled(bProjectExists && !bProcessing);
  mActionExportTiePoints->setEnabled(bProjectExists && bFeatureExtraction && !bProcessing);
  mActionExportMatches->setEnabled(bProjectExists && bFeatureMatching && !bProcessing);
  mActionExportOrientations->setEnabled(bProjectExists && bOriented && !bProcessing);
  mActionExportPointCloud->setEnabled(bProjectExists && bDenseModel && !bProcessing);
  mActionSaveProject->setEnabled(bProjectExists && bProjectModified && !bProcessing);
  mActionSaveProjectAs->setEnabled(bProjectExists && !bProcessing);
  mActionCloseProject->setEnabled(bProjectExists && !bProcessing);
  mActionExit->setEnabled(!bProcessing);

  mActionLoadImages->setEnabled(bProjectExists && !bProcessing && !bLoadingImages);
  mActionFeatureExtraction->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
  mActionFeatureMatching->setEnabled(bProjectExists && bFeatureExtraction && !bProcessing);
  mActionOrientation->setEnabled(bProjectExists && bFeatureMatching && !bProcessing);
  mActionDensification->setEnabled(bProjectExists && bOriented && !bProcessing);

  mActionCameras->setEnabled(bProjectExists && bImagesLoaded);
  mActionDtm->setEnabled(bProjectExists && bAbsoluteOriented && !bProcessing);
  mActionGeoreference->setEnabled(bProjectExists && bOriented && !bProcessing);
  mActionFeaturesViewer->setEnabled(bFeatureExtraction);
  mActionMatchesViewer->setEnabled(bFeatureMatching);
  //mActionPassPointsViewer->setEnabled(bFeatureMatching);
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
  mActionExportOrientations->setText(QApplication::translate("MainWindowView", "Export Orientations", nullptr));
  mActionExportPointCloud->setText(QApplication::translate("MainWindowView", "Export Point Cloud", nullptr));
  mActionCloseProject->setText(QApplication::translate("MainWindowView", "Close Project", nullptr));
  mActionExit->setText(QApplication::translate("MainWindowView", "Exit", nullptr));
  mActionStartPage->setText(QApplication::translate("MainWindowView", "Start Page", nullptr));
  mActionLoadImages->setText(QApplication::translate("MainWindowView", "Load Images", nullptr));
  mActionFeatureExtraction->setText(QApplication::translate("MainWindowView", "Feature Extraction", nullptr));
  mActionFeatureMatching->setText(QApplication::translate("MainWindowView", "Feature Matching", nullptr));
  mActionOrientation->setText(QApplication::translate("MainWindowView", "Orientation", nullptr));
  mActionDensification->setText(QApplication::translate("MainWindowView", "Densification", nullptr));
  mActionCameras->setText(QApplication::translate("MainWindowView", "Cameras", nullptr));
  mActionGeoreference->setText(QApplication::translate("MainWindowView", "Georeference", nullptr));
  mActionHelp->setText(QApplication::translate("MainWindowView", "Help", nullptr));
  mActionAbout->setText(QApplication::translate("MainWindowView", "About", nullptr));
  mActionFeaturesViewer->setText(QApplication::translate("MainWindowView", "Keypoints Viewer", nullptr));
  mActionMatchesViewer->setText(QApplication::translate("MainWindowView", "Matches Viewer", nullptr));
  //mActionPassPointsViewer->setText(QApplication::translate("MainWindowView", "Multiview Matching Assessment", nullptr));
  mActionSettings->setText(QApplication::translate("MainWindowView", "Settings", nullptr));
  mActionNotRecentProjects->setText(QApplication::translate("MainWindowView", "Not recent projects", nullptr));
  mActionClearHistory->setText(QApplication::translate("MainWindowView", "Clear History", nullptr));
  mActionOpenImage->setText(QApplication::translate("MainWindowView", "Open Image", nullptr));
  mActionDeleteImage->setText(QApplication::translate("MainWindowView", "Delete Image", nullptr));
  mActionViewKeypoints->setText(QApplication::translate("MainWindowView", "View Keypoints", nullptr));
  mActionViewMatches->setText(QApplication::translate("MainWindowView", "View Matches", nullptr));
  mActionDtm->setText(QApplication::translate("MainWindowView", "DTM/DSM", nullptr));


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
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Inspector");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
}

} // namespace ui

} // namespace inspector
