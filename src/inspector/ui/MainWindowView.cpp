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

namespace inspector
{

namespace ui
{

enum
{
  project,
  images,
  image,
  features,
  features_images,
  features_image,
  detector,
  descriptor,
  matches,
  descriptor_matcher,
  matches_pairs,
  pair_left,
  pair_right
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
    mActionSettings(new QAction(this)),
    mActionHelp(new QAction(this)),
    mActionAbout(new QAction(this)),
    mActionExportTiePoints(new QAction(this)),
    mActionExportMatches(new QAction(this)),
    mActionFeaturesViewer(new QAction(this)),
    mActionMatchesViewer(new QAction(this)),
    mActionPassPointsViewer(new QAction(this)),
    mActionNotRecentProjects(new QAction(this)),
    mActionClearHistory(new QAction(this)),
    mComboBoxActiveSession(new QComboBox(this)),
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

void MainWindowView::addImages(const QStringList &images)
{
  if (images.empty() == false) {
//    for (auto &image : images) {
//      if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//        /* Fotogramas */

//        QTreeWidgetItem *itemImages = nullptr;
//        for (int i = 0; i < itemProject->childCount(); i++) {
//          QTreeWidgetItem *temp = itemProject->child(i);
//          if (temp->text(0).compare(tr("Images")) == 0) {
//            itemImages = itemProject->child(i);
//            break;
//          }
//        }

//        if (itemImages == nullptr) {
//          itemImages = new QTreeWidgetItem();
//          itemImages->setText(0, tr("Images"));
//          itemImages->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_documents_folder_48px.png"));
//          itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
//          itemImages->setData(0, Qt::UserRole, inspector::ui::images);
//          itemProject->addChild(itemImages);
//          itemImages->setExpanded(true);
//        }

//        /* Se añade el fotograma al árbol del proyecto */
//        QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
//        itemPhotogram->setText(0, QFileInfo(image).baseName());
//        itemPhotogram->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_image_48px.png"));
//        itemPhotogram->setToolTip(0, image);
//        itemPhotogram->setData(0, Qt::UserRole, inspector::ui::image);
//        itemImages->addChild(itemPhotogram);

//        update();
//      }
//    }

    mThumbnailsWidget->addThumbnails(images);

  }
}

void MainWindowView::setActiveImage(const QString &image)
{
//  const QSignalBlocker blocker1(mTreeWidgetProject);
//  const QSignalBlocker blocker2(mThumbnailsWidget);

//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    QTreeWidgetItem *itemImages = nullptr;
//    for (int i = 0; i < itemProject->childCount(); i++) {
//      QTreeWidgetItem *temp = itemProject->child(i);
//      if (temp->text(0).compare(tr("Images")) == 0) {
//        itemImages = itemProject->child(i);
//        break;
//      }
//    }

//    if (itemImages) {
//      // Se busca la imagen en el árbol
//      for (int i = 0; i < itemImages->childCount(); i++) {
//        QTreeWidgetItem *temp = itemImages->child(i);
//        itemImages->child(i)->setSelected(temp->toolTip(0).compare(image) == 0);
//      }
//    }
//  }

//  mThumbnailsWidget->setActiveImage(image);

}

void MainWindowView::setActiveImages(const QStringList &images)
{
//  const QSignalBlocker blocker1(mTreeWidgetProject);
//  const QSignalBlocker blocker2(mThumbnailsWidget);

//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    QTreeWidgetItem *itemImages = nullptr;
//    for (int i = 0; i < itemProject->childCount(); i++) {
//      QTreeWidgetItem *temp = itemProject->child(i);
//      if (temp->text(0).compare(tr("Images")) == 0) {
//        itemImages = itemProject->child(i);
//        break;
//      }
//    }

//    if (itemImages) {
//      // Se busca la imagen en el árbol
//      for (int i = 0; i < itemImages->childCount(); i++) {
//        QTreeWidgetItem *temp = itemImages->child(i);
//        temp->setSelected(false);
//        for (auto &image : images){
//          if (temp->toolTip(0).compare(image) == 0){
//            temp->setSelected(true);
//            break;
//          }
//        }
//      }
//    }
//  }

//  mThumbnailsWidget->setActiveImages(images);
}

void MainWindowView::addFeatures(const QString &sessionName, const QString &detector, const QString &descriptor, const std::vector<QString> &features)
{
//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    /* Sessions */

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
//          if (itemSession->text(0).compare(sessionName) == 0){

//            for (int j = 0; j < itemSession->childCount(); j++) {
//              if (QTreeWidgetItem *temp = itemSession->child(j)){
//                if (temp->text(0).compare(tr("Features")) == 0) {
//                  itemFeatures = itemSession->child(j);
//                  break;
//                }
//              }
//            }

//            if (itemFeatures == nullptr) {
//              itemFeatures = new QTreeWidgetItem();
//              itemFeatures->setText(0, QString("Features"));
//              itemFeatures->setData(0, Qt::UserRole, inspector::ui::features);
//              itemSession->addChild(itemFeatures);
//            }

//            update();
//            break;

//          }
//        }
//      }

//      if (itemFeatures != nullptr){

//        /// Detector
//        QTreeWidgetItem *itemDetector = nullptr;
//        for (int i = 0; i < itemFeatures->childCount(); i++) {
//          if (QTreeWidgetItem *temp = itemFeatures->child(i)){
//            QStringList l = temp->text(0).split(":");
//            if (l.size() == 2){
//              if (l.at(0).compare(tr("Detector")) == 0) {
//                itemDetector = itemFeatures->child(i);
//                break;
//              }
//            }
//          }

//        }

//        if (itemDetector == nullptr) {
//          itemDetector = new QTreeWidgetItem();
//          itemDetector->setData(0, Qt::UserRole, inspector::ui::detector);
//          itemFeatures->addChild(itemDetector);
//        }

//        itemDetector->setText(0, QString("Detector: ").append(detector));


//        /// Descriptor
//        QTreeWidgetItem *itemDescriptor = nullptr;
//        for (int i = 0; i < itemFeatures->childCount(); i++) {
//          if (QTreeWidgetItem *temp = itemFeatures->child(i)){
//            QStringList l = temp->text(0).split(":");
//            if (l.size() == 2){
//              if (l.at(0).compare(tr("Descriptor")) == 0) {
//                itemDescriptor = itemFeatures->child(i);
//                break;
//              }
//            }
//          }
//        }

//        if (itemDescriptor == nullptr) {
//          itemDescriptor = new QTreeWidgetItem();
//          itemDescriptor->setData(0, Qt::UserRole, inspector::ui::descriptor);
//          itemFeatures->addChild(itemDescriptor);
//        }

//        itemDescriptor->setText(0, QString("Descriptor: ").append(descriptor));

//        QTreeWidgetItem *itemImagesFeatures = nullptr;
//        for (int i = 0; i < itemFeatures->childCount(); i++) {
//          QTreeWidgetItem *temp = itemFeatures->child(i);
//          if (temp && temp->text(0).compare(tr("Keypoints")) == 0) {
//            itemImagesFeatures = itemFeatures->child(i);
//            break;
//          }
//        }

//        if (itemImagesFeatures == nullptr) {
//          itemImagesFeatures = new QTreeWidgetItem();
//          itemImagesFeatures->setText(0, tr("Keypoints"));
//          itemImagesFeatures->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_documents_folder_48px.png"));
//          itemImagesFeatures->setFlags(itemImagesFeatures->flags() | Qt::ItemIsTristate);
//          itemImagesFeatures->setData(0, Qt::UserRole, inspector::ui::features_images);
//          itemFeatures->addChild(itemImagesFeatures);
//        }

//        for (auto &image_features : features) {

//          QTreeWidgetItem *itemImageFeatures = nullptr;
//          for (int i = 0; i < itemImagesFeatures->childCount(); i++) {
//            QTreeWidgetItem *temp = itemImagesFeatures->child(i);
//            if (temp && temp->toolTip(0).compare(image_features) == 0) {
//              itemImageFeatures = itemImagesFeatures->child(i);
//              break;
//            }
//          }

//          if (itemImageFeatures == nullptr) {
//            itemImageFeatures = new QTreeWidgetItem();
//            itemImageFeatures->setText(0, QFileInfo(image_features).baseName());
//            itemImageFeatures->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_xml_48px.png"));
//            itemImageFeatures->setToolTip(0, image_features);
//            itemImageFeatures->setFlags(itemImageFeatures->flags() | Qt::ItemIsTristate);
//            itemImageFeatures->setData(0, Qt::UserRole, inspector::ui::features_image);
//            itemImagesFeatures->addChild(itemImageFeatures);
//          }

//        }

//      }

//    }
//  }
}

void MainWindowView::addMatches(const QString &sessionName, const QString &matcher, const QString &left, const QString &right, const QString &file)
{
//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    /* Sessions */

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
//          if (itemSession->text(0).compare(sessionName) == 0){

//            for (int j = 0; j < itemSession->childCount(); j++) {
//              if (QTreeWidgetItem *temp = itemSession->child(j)){
//                if (temp->text(0).compare(tr("Matches")) == 0) {
//                  itemMatches = itemSession->child(j);
//                  break;
//                }
//              }
//            }

//            if (itemMatches == nullptr) {
//              itemMatches = new QTreeWidgetItem();
//              itemMatches->setText(0, QString("Matches"));
//              itemMatches->setData(0, Qt::UserRole, inspector::ui::matches);
//              itemSession->addChild(itemMatches);
//            }

//            update();
//            break;

//          }
//        }
//      }

//      if (itemMatches != nullptr){

//        /// Detector
//        QTreeWidgetItem *itemMatcher = nullptr;
//        for (int i = 0; i < itemMatches->childCount(); i++) {
//          if (QTreeWidgetItem *temp = itemMatches->child(i)){
//            QStringList l = temp->text(0).split(":");
//            if (l.size() == 2){
//              if (l.at(0).compare(tr("Descriptor Matcher")) == 0) {
//                itemMatcher = itemMatches->child(i);
//                break;
//              }
//            }
//          }

//        }

//        if (itemMatcher == nullptr) {
//          itemMatcher = new QTreeWidgetItem();
//          itemMatcher->setData(0, Qt::UserRole, inspector::ui::descriptor_matcher);
//          itemMatches->addChild(itemMatcher);
//        }

//        itemMatcher->setText(0, QString("Descriptor Matcher: ").append(matcher));


//        /* Image pairs */

//        QTreeWidgetItem *itemImagePairs = nullptr;
//        for (int i = 0; i < itemMatches->childCount(); i++) {
//          QTreeWidgetItem *temp = itemMatches->child(i);
//          if (temp && temp->text(0).compare(tr("Image Pairs")) == 0) {
//            itemImagePairs = itemMatches->child(i);
//            break;
//          }
//        }

//        if (itemImagePairs == nullptr) {
//          itemImagePairs = new QTreeWidgetItem();
//          itemImagePairs->setText(0, tr("Image Pairs"));
//          itemImagePairs->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_documents_folder_48px.png"));
//          itemImagePairs->setFlags(itemImagePairs->flags() | Qt::ItemIsTristate);
//          itemImagePairs->setData(0, Qt::UserRole, inspector::ui::matches_pairs);
//          itemMatches->addChild(itemImagePairs);
//          itemImagePairs->setExpanded(true);
//        }

//        QTreeWidgetItem *itemLeftImage = nullptr;
//        for (int i = 0; i < itemImagePairs->childCount(); i++) {
//          QTreeWidgetItem *temp = itemImagePairs->child(i);
//          if (temp && temp->text(0).compare(left) == 0) {
//            itemLeftImage = itemImagePairs->child(i);
//            break;
//          }
//        }

//        if (itemLeftImage == nullptr) {
//          itemLeftImage = new QTreeWidgetItem();
//          itemLeftImage->setText(0, left);
//          itemLeftImage->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_stack_of_photos_48px.png"));
//          itemLeftImage->setFlags(itemLeftImage->flags() | Qt::ItemIsTristate);
//          itemLeftImage->setData(0, Qt::UserRole, inspector::ui::pair_left);
//          itemImagePairs->addChild(itemLeftImage);
//        }

//        QTreeWidgetItem *itemRightImage = nullptr;
//        for (int i = 0; i < itemLeftImage->childCount(); i++) {
//          QTreeWidgetItem *temp = itemLeftImage->child(i);
//          if (temp && temp->text(0).compare(right) == 0) {
//            itemRightImage = itemLeftImage->child(i);
//            break;
//          }
//        }

//        if (itemRightImage == nullptr) {
//          itemRightImage = new QTreeWidgetItem();
//          itemRightImage->setText(0, right);
//          itemRightImage->setToolTip(0, file);
//          itemRightImage->setIcon(0, QIcon(":/ico/48/img/material/48/icons8_image_48px.png"));
//          itemRightImage->setFlags(itemRightImage->flags() | Qt::ItemIsTristate);
//          itemRightImage->setData(0, Qt::UserRole, inspector::ui::pair_right);
//          itemLeftImage->addChild(itemRightImage);
//        }

//      }

//    }
//  }
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
//  if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

//    QTreeWidgetItem *itemImages = nullptr;
//    for (int i = 0; i < itemProject->childCount(); i++) {
//      QTreeWidgetItem *temp = itemProject->child(i);
//      if (temp->text(0).compare(tr("Images")) == 0) {
//        itemImages = itemProject->child(i);
//        break;
//      }
//    }

//    if (itemImages) {

//      // Se busca la imagen en el árbol
//      QTreeWidgetItem *itemImage = nullptr;
//      for (int i = 0; i < itemImages->childCount(); i++) {
//        QTreeWidgetItem *temp = itemImages->child(i);
//        if (temp->toolTip(0).compare(file) == 0) {
//          itemImage = itemImages->child(i);
//          delete itemImage;
//          itemImage = nullptr;
//          break;
//        }
//      }
//    }

//    setFlag(MainWindowView::Flag::images_added, itemProject->childCount() > 0);
//  }

//  mThumbnailsWidget->deleteThumbnail(file);
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

void MainWindowView::update()
{
  bool bProjectExists = mFlags.isActive(Flag::project_exists);
  bool bProjectModified = mFlags.isActive(Flag::project_modified);
//  bool bImageOpen = mFlags.isActive(Flag::image_open);
  bool bProcessing = mFlags.isActive(Flag::processing);
//  bool bLoadingImages = mFlags.isActive(Flag::loading_images);

  mActionNewProject->setEnabled(!bProcessing);
  mActionOpenProject->setEnabled(!bProcessing);
//  mMenuRecentProjects->setEnabled(!bProcessing);
  mActionSaveProject->setEnabled(bProjectExists && bProjectModified && !bProcessing);
  mActionSaveProjectAs->setEnabled(bProjectExists && !bProcessing);
  mActionCloseProject->setEnabled(bProjectExists && !bProcessing);
  mActionExit->setEnabled(!bProcessing);


//  mActionLoadImages->setEnabled(bProjectExists && !bProcessing);
//  mActionGroundTruthEditor->setEnabled(mFlags.isActive(Flag::images_added));
//  mActionPreprocess->setEnabled(mFlags.isActive(Flag::session_created) && !bProcessing);
//  mActionFeatureExtraction->setEnabled(mFlags.isActive(Flag::session_created) && !bProcessing);
//  mActionFeatureMatching->setEnabled(mFlags.isActive(Flag::feature_extraction) && !bProcessing);
//  //mActionBatch->setEnabled(mFlags.isActive(Flag::session_created) && !bProcessing);
//  mActionExportTiePoints->setEnabled(mFlags.isActive(Flag::session_created) && !bProcessing);
//  mActionExportMatches->setEnabled(mFlags.isActive(Flag::session_created) && !bProcessing);
//  mActionFeaturesViewer->setEnabled(mFlags.isActive(Flag::feature_extraction));
//  mActionMatchesViewer->setEnabled(mFlags.isActive(Flag::feature_matching));
//  mActionPassPointsViewer->setEnabled(mFlags.isActive(Flag::feature_matching));

//  mActionNotRecentProjects->setVisible(mHistory.size() == 0);
//  mActionClearHistory->setEnabled(mHistory.size() > 0);

//  mActionLoadImages->setDisabled(bLoadingImages || bProcessing);
}

void MainWindowView::retranslate()
{
  mActionNewProject->setText(QApplication::translate("MainWindowView", "New Project", nullptr));
  mActionOpenProject->setText(QApplication::translate("MainWindowView", "Open Project", nullptr));
  mActionSaveProject->setText(QApplication::translate("MainWindowView", "Save Project", nullptr));
  mActionSaveProjectAs->setText(QApplication::translate("MainWindowView", "Save Project As...", nullptr));
  mActionExportTiePoints->setText(QApplication::translate("MainWindowView", "Export tie points", nullptr));
  mActionExportMatches->setText(QApplication::translate("MainWindowView", "Export Matches", nullptr));
  mActionCloseProject->setText(QApplication::translate("MainWindowView", "Close Project", nullptr));
  mActionExit->setText(QApplication::translate("MainWindowView", "Exit", nullptr));
  mActionStartPage->setText(QApplication::translate("MainWindowView", "Start Page", nullptr));
  mActionLoadImages->setText(QApplication::translate("MainWindowView", "Load Images", nullptr));
  mActionFeatureExtraction->setText(QApplication::translate("MainWindowView", "Feature Extraction", nullptr));
  mActionFeatureMatching->setText(QApplication::translate("MainWindowView", "Feature Matching", nullptr));
  mActionOrientation->setText(QApplication::translate("MainWindowView", "Orientation", nullptr));
  mActionDensification->setText(QApplication::translate("MainWindowView", "Densification", nullptr));
  mActionHelp->setText(QApplication::translate("MainWindowView", "Help", nullptr));
  mActionAbout->setText(QApplication::translate("MainWindowView", "About", nullptr));
  mActionFeaturesViewer->setText(QApplication::translate("MainWindowView", "Keypoints Viewer", nullptr));
  mActionMatchesViewer->setText(QApplication::translate("MainWindowView", "Matches Viewer", nullptr));
  mActionPassPointsViewer->setText(QApplication::translate("MainWindowView", "Multiview Matching Assessment", nullptr));
  mActionSettings->setText(QApplication::translate("MainWindowView", "Settings", nullptr));
  mActionNotRecentProjects->setText(QApplication::translate("MainWindowView", "Not recent projects", nullptr));
  mActionClearHistory->setText(QApplication::translate("MainWindowView", "Clear History", nullptr));
  mMenuRecentProjects->setTitle(QApplication::translate("MainWindowView", "Recent Projects", nullptr));
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

void MainWindowView::openFromHistory()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    emit openProjectFromHistory(action->data().toString());
}

void MainWindowView::onSelectionChanged()
{
//  QList<QTreeWidgetItem*> item = mTreeWidgetProject->selectedItems();
//  if (item.isEmpty()) return;
//  if (item[0]->data(0, Qt::UserRole) == inspector::ui::project){

//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::images){
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::image ||
//             item[0]->data(0, Qt::UserRole) == inspector::ui::preprocess_image){
//    int size = item.size();
//    if(size > 0){
//      if (size == 1) {
//        emit selectImage(item[0]->toolTip(0));
//      } else {
//        QStringList selected_images;
//        for (int i = 0; i < size; i++){
//          selected_images.push_back(item[i]->toolTip(0));
//        }
//        emit selectImages(selected_images);
//      }
//    }
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::sessions){
//    ui->treeWidgetProperties->clear();
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::session){
//    int size = item.size();
//    if(size > 0){
//      if (size == 1) {
//        emit selectSession(item[0]->text(0));
//      } /*else {
//        QStringList selected_images;
//        for (int i = 0; i < size; i++){
//          selected_images.push_back(item[i]->toolTip(0));
//        }
//        emit selectImages(selected_images);
//      }*/
//    }
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::preprocess){
//    int size = item.size();
//    if(size > 0){
//      if (size == 1) {
//        QString session = item[0]->parent()->text(0);
//        emit selectPreprocess(session);
//      }
//    }
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::features){
//    ui->treeWidgetProperties->clear();
//    QString session = item[0]->parent()->text(0);
//    emit selectFeatures(session);
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::features_images){
//    ui->treeWidgetProperties->clear();
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::features_image){
//    ui->treeWidgetProperties->clear();
//    emit selectImageFeatures(item[0]->toolTip(0));
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::detector){
//    int size = item.size();
//    if(size > 0){
//      if (size == 1) {
//        QString session = item[0]->parent()->parent()->text(0);
//        emit selectDetector(session);
//      }
//    }
//  } else if (item[0]->data(0, Qt::UserRole) == inspector::ui::descriptor){
//    int size = item.size();
//    if(size > 0){
//      if (size == 1) {
//        QString session = item[0]->parent()->parent()->text(0);
//        emit selectDescriptor(session);
//      }
//    }
//  }

////  if (item[0]->parent()->text(0).compare(tr("Images")) == 0){
////    int size = item.size();
////    if(size > 0){
////      if (size == 1) {
////        emit selectImage(item[0]->toolTip(0));
////      } else {
////        QStringList selected_images;
////        for (int i = 0; i < size; i++){
////          selected_images.push_back(item[i]->toolTip(0));
////        }
////        emit selectImages(selected_images);
////      }
////    }
////  } else if (item[0]->parent()->text(0).compare(tr("Sessions")) == 0){
////    int size = item.size();
////    if(size > 0){
////      if (size == 1) {
////        emit selectSession(item[0]->text(0));
////      } /*else {
////        QStringList selected_images;
////        for (int i = 0; i < size; i++){
////          selected_images.push_back(item[i]->toolTip(0));
////        }
////        emit selectImages(selected_images);
////      }*/
////    }
//  //  }
}

void MainWindowView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
//  if (item){
//    if (item->data(0, Qt::UserRole) == inspector::ui::image ||
//      item->data(0, Qt::UserRole) == inspector::ui::preprocess_image){
//     emit openImage(item->toolTip(column));
//   } else if (item->data(0, Qt::UserRole) == inspector::ui::pair_right){
//     QString session = item->parent()->parent()->parent()->parent()->text(0);
//     emit openImageMatches(session, item->parent()->text(0), item->text(column));
//   } else if (item->data(0, Qt::UserRole) == inspector::ui::features_image){
//     QString session = item->parent()->parent()->parent()->text(0);
//     emit openKeypointsViewer(session, QFileInfo(item->text(column)).baseName());
//   }
//  }
}

void MainWindowView::onTreeContextMenu(const QPoint &point)
{
//  QPoint globalPos = mTreeWidgetProject->mapToGlobal(point);

//  QTreeWidgetItem *item = nullptr;
//  item = mTreeWidgetProject->itemAt(point);

//  if (item == nullptr)
//    return;

//  if (item->data(0, Qt::UserRole) == inspector::ui::project){
//    QMenu menu;
//    menu.addAction(mActionLoadImages);

//    QAction *selectedItem = menu.exec(globalPos);
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::images){
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::image ||
//             item->data(0, Qt::UserRole) == inspector::ui::preprocess_image){
//    QMenu menu;
//    QAction *open_image = new QAction(QIcon(QStringLiteral(":/ico/24/img/material/24/icons8_image_file_24px.png")),
//                                       tr("Open Image"), this);
//    menu.addAction(open_image);
//    QAction *delete_image = new QAction(QIcon(QStringLiteral(":/ico/24/img/material/24/icons8_remove_image_24px.png")),
//                                       tr("Delete Image"), this);
//    menu.addAction(delete_image);
//    if (QAction *selectedItem = menu.exec(globalPos)) {
//      if (selectedItem->text() == tr("Open Image")) {
//        emit openImage(item->toolTip(0));
//      } else if (selectedItem->text() == tr("Delete Image")) {
//        emit deleteImages(QStringList(item->toolTip(0)));
//      }
//    }
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::sessions){
//    QMenu menu;
//    menu.addAction(mActionNewSession);

//    QAction *selectedItem = menu.exec(globalPos);
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::session){
//    QMenu menu;
//    menu.addAction(mActionSetSession);
//    menu.addAction(mActionDeleteSession);
////    menu.addSeparator();
////    menu.addAction(mActionPreprocess);
////    menu.addAction(mActionFeatureExtraction);
////    menu.addAction(mActionFeatureMatching);

//    if (QAction *selectedItem = menu.exec(globalPos)) {
//      if (selectedItem->text() == tr("Set as current session")) {
//        emit activeSessionChange(item->text(0));
//      } else if (selectedItem->text() == tr("Delete session")) {
//        emit delete_session(item->text(0));
//      }
//    }
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::preprocess){

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::features){

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::features_images){

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::features_image){
//    QMenu menu;
//    menu.addAction(tr("View keypoints"));
//    if (QAction *selectedItem = menu.exec(globalPos)) {
//      if (selectedItem->text() == tr("View keypoints")) {
//        QString session = item->parent()->parent()->parent()->text(0);
//        emit openKeypointsViewer(session, QFileInfo(item->text(0)).baseName());
//      }
//    }

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::detector){

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::descriptor){

//  } else if (item->data(0, Qt::UserRole) == inspector::ui::pair_left){
//    QMenu menu;
//    menu.addAction(tr("View Matches"));
//    if (QAction *selectedItem = menu.exec(globalPos)) {
//      if (selectedItem->text() == tr("View Matches")) {
//        QString session = item->parent()->parent()->parent()->text(0);
//        emit openMatchesViewer(session, QFileInfo(item->text(0)).baseName(), QString());
//      }
//    }
//  } else if (item->data(0, Qt::UserRole) == inspector::ui::pair_right){
//    QMenu menu;
//    menu.addAction(tr("View Matches"));
//    if (QAction *selectedItem = menu.exec(globalPos)) {
//      if (selectedItem->text() == tr("View Matches")) {
//        QString session = item->parent()->parent()->parent()->parent()->text(0);
//        emit openMatchesViewer(session, QFileInfo(item->parent()->text(0)).baseName(), QFileInfo(item->text(0)).baseName());
//      }
//    }
//  }

}

void MainWindowView::initUI()
{
  setWindowTitle(QString("Inspector Image"));

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

//  QIcon iconFeatureExtraction;
//  iconFeatureExtraction.addFile(QStringLiteral(":/ico/24/img/material/24/features.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionFeatureExtraction->setIcon(iconFeatureExtraction);

//  QIcon iconFeatureMatching;
//  iconFeatureMatching.addFile(QStringLiteral(":/ico/24/img/material/24/match_view.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionFeatureMatching->setIcon(iconFeatureMatching);

//  QIcon iconOrientation;
//  iconOrientation.addFile(QStringLiteral(":/ico/24/img/material/24/match_view.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionOrientation->setIcon(iconOrientation);

//  QIcon iconDensification;
//  iconDensification.addFile(QStringLiteral(":/ico/24/img/material/24/match_view.png"), QSize(), QIcon::Normal, QIcon::Off);
//  mActionDensification->setIcon(iconDensification);

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

  /* Árbol de proyecto */
  mTreeWidgetProject = new QTreeWidget(ui->dockWidgetContentsProject);
  mTreeWidgetProject->setContextMenuPolicy(Qt::CustomContextMenu);
  mTreeWidgetProject->header()->close();
  mTreeWidgetProject->setColumnCount(1);
  mTreeWidgetProject->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  mTreeWidgetProject->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  mTreeWidgetProject->header()->setStretchLastSection(false);
  ui->gridLayout->addWidget(mTreeWidgetProject, 0, 0, 1, 1);

  /* Thumbnails */
  QGridLayout *gridLayoutThumb = new QGridLayout(ui->dockWidgetThumbContents);
  gridLayoutThumb->setSpacing(6);
  gridLayoutThumb->setContentsMargins(11, 11, 11, 11);
  gridLayoutThumb->setContentsMargins(0, 0, 0, 0);
  mThumbnailsWidget = new ThumbnailsWidget(ui->dockWidgetThumbContents);
  gridLayoutThumb->addWidget(mThumbnailsWidget, 0, 0, 1, 1);

  /* Log */
  QGridLayout *gridLayoutConsole = new QGridLayout(ui->dockWidgetContentsConsole);
  gridLayoutConsole->setSpacing(6);
  gridLayoutConsole->setContentsMargins(11, 11, 11, 11);
  gridLayoutConsole->setContentsMargins(0, 0, 0, 0);
  mLogWidget = new LogWidget(ui->dockWidgetThumbContents);
  gridLayoutConsole->addWidget(mLogWidget, 0, 0, 1, 1);

  /* Tab Handler */
  QGridLayout *gridLayoutCentral = new QGridLayout(this->centralWidget());
  gridLayoutCentral->setSpacing(6);
  gridLayoutCentral->setContentsMargins(0,0,0,0);
  mTabHandler = new TabHandler(this->centralWidget());
  gridLayoutCentral->addWidget(mTabHandler);

  /* Menu file */

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
  mMenuExport = new QMenu(this);
  mMenuExport->addAction(mActionExportTiePoints);
  mMenuExport->addAction(mActionExportMatches);
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

  /* Menu View */

  ui->menuView->addAction(mActionStartPage);
  ui->menuView->addSeparator();
  mMenuPanels = new QMenu(this);
  mMenuPanels->addAction(ui->dockWidgetProject->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetProperties->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetConsole->toggleViewAction());
  mMenuPanels->addAction(ui->dockWidgetThumb->toggleViewAction());
  ui->menuView->addMenu(mMenuPanels);

  ui->menuView->addSeparator();

  mMenuToolBar = new QMenu(this);
  mMenuToolBar->addAction(ui->mainToolBar->toggleViewAction());
  mMenuToolBar->addAction(ui->toolBarTools->toggleViewAction());
  mMenuToolBar->addAction(ui->toolBarView->toggleViewAction());
  mMenuToolBar->addAction(ui->toolBarWorkflow->toggleViewAction());
  ui->menuView->addMenu(mMenuToolBar);

  /* Menu Workflow */

  ui->menuWorkflow->addAction(mActionLoadImages);
  ui->menuWorkflow->addSeparator();
  ui->menuWorkflow->addAction(mActionFeatureExtraction);
  ui->menuWorkflow->addAction(mActionFeatureMatching);
  ui->menuWorkflow->addAction(mActionOrientation);
  ui->menuWorkflow->addAction(mActionDensification);

  /* Menu Tools */

  ui->menuTools->addAction(mActionFeaturesViewer);
  ui->menuTools->addAction(mActionMatchesViewer);
  ui->menuTools->addAction(mActionPassPointsViewer);
  ui->menuTools->addSeparator();
  ui->menuTools->addAction(mActionSettings);

  /* Menu Help */

  ui->menuHelp->addAction(mActionHelp);
  ui->menuHelp->addSeparator();
  ui->menuHelp->addAction(mActionAbout);

  /* Toolbars */

  ui->mainToolBar->addAction(mActionNewProject);
  ui->mainToolBar->addAction(mActionOpenProject);
  ui->mainToolBar->addAction(mActionSaveProject);
  ui->mainToolBar->addAction(mActionSaveProjectAs);

  ui->toolBarTools->addAction(mActionLoadImages);
  ui->toolBarTools->addSeparator();
  ui->toolBarTools->addAction(mActionFeatureExtraction);
  ui->toolBarTools->addAction(mActionFeatureMatching);

  ui->toolBarView->addAction(mTabHandler->actionZoomIn());
  ui->toolBarView->addAction(mTabHandler->actionZoomOut());
  ui->toolBarView->addAction(mTabHandler->actionZoom11());
  ui->toolBarView->addAction(mTabHandler->actionZoomExtend());

  mLayoutCentral = new QGridLayout(this->centralWidget());
  mLayoutCentral->setSpacing(6);
  mLayoutCentral->setContentsMargins(0,0,0,0);

  mProgressBar = new QProgressBar(this);
  mProgressBar->setVisible(false);
  mProgressBar->setMaximumSize(150, 20);
  ui->statusBar->addPermanentWidget(mProgressBar);

  // Configuración de mensajes
  tl::MessageManager &msg_h = tl::MessageManager::instance();
  msg_h.addListener(mLogWidget);

  this->retranslate();
  this->update();
}

void MainWindowView::initSignalAndSlots()
{
  /* Menú Archivo */

  connect(mActionNewProject,           SIGNAL(triggered(bool)), this,   SIGNAL(openNew()));
  connect(mActionOpenProject,          SIGNAL(triggered(bool)), this,   SIGNAL(openProject()));
  connect(mActionClearHistory,         SIGNAL(triggered(bool)), this,   SIGNAL(clearHistory()));
  connect(mActionSaveProject,          SIGNAL(triggered(bool)), this,   SIGNAL(saveProject()));
  connect(mActionSaveProjectAs,        SIGNAL(triggered(bool)), this,   SIGNAL(saveProjectAs()));
  connect(mActionExportTiePoints,      SIGNAL(triggered(bool)), this,   SIGNAL(openExportFeatures()));
  connect(mActionExportMatches,        SIGNAL(triggered(bool)), this,   SIGNAL(openExportMatches()));
  connect(mActionCloseProject,         SIGNAL(triggered(bool)), this,   SIGNAL(closeProject()));
  connect(mActionExit,                 SIGNAL(triggered(bool)), this,   SIGNAL(exit()));

  /* Menú View */

  connect(mActionStartPage,          SIGNAL(triggered(bool)),   this,   SIGNAL(openStartPage()));

  /* Menú Flujo de trabajo */

  connect(mActionLoadImages,         SIGNAL(triggered(bool)),   this,   SIGNAL(loadImages()));
  connect(mActionFeatureExtraction,  SIGNAL(triggered(bool)),   this,   SIGNAL(openFeatureExtraction()));
  connect(mActionFeatureMatching,    SIGNAL(triggered(bool)),   this,   SIGNAL(openFeatureMatching()));

  /* Menú herramientas */

  connect(mActionFeaturesViewer,     SIGNAL(triggered(bool)),   this,   SIGNAL(openKeypointsViewer()));
  connect(mActionMatchesViewer,      SIGNAL(triggered(bool)),   this,   SIGNAL(openMatchesViewer()));
  connect(mActionPassPointsViewer,   SIGNAL(triggered(bool)),   this,   SIGNAL(openMultiviewMatchingAssessment()));
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

void MainWindowView::closeEvent(QCloseEvent *event)
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Inspector");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QMainWindow::closeEvent(event);
}

} // namespace ui

} // namespace inspector
