#include "ThumbnailsWidget.h"

#include <QFileInfo>
#include <QListWidget>
#include <QGridLayout>
#include <QToolBar>
#include <QtConcurrent/QtConcurrentMap>
#include <QFutureWatcher>
#include <QImage>
#include <mutex>
#include <QImageReader>
#include <QApplication>

static std::mutex sMutexThumbnail;

QImage makeThumbnail(const QString &thumb)
{  
  std::lock_guard<std::mutex> lck(sMutexThumbnail);

  QImageReader imageReader(thumb);
  QSize size = imageReader.size();
  double scale = 1.;
  int w = size.width();
  int h = size.height();
  if (w > h){
    scale = w / 200.;
  } else {
    scale = h / 200.;
  }
  size /= scale;
  imageReader.setScaledSize(size);
  QImage image_scaled = imageReader.read();

  return image_scaled;
}

namespace inspector
{

ThumbnailsWidget::ThumbnailsWidget(QWidget *parent)
  : IWidgetView(parent),
    mListWidget(new QListWidget(this)),
    mGridLayout(new QGridLayout(this)),
    mThumbnailAction(nullptr),
    mDetailsAction(nullptr),
    mDeleteImageAction(nullptr),
    mThumbnaislSize(0)
{
  this->initUI();
  this->initSignalAndSlots();
}

void ThumbnailsWidget::setActiveImage(const QString &image)
{
  QListWidgetItem *item = nullptr;
  for (int i = 0; i < mListWidget->count(); i++){
   item = mListWidget->item(i);
   item->setSelected(item->toolTip().compare(image) == 0);
  }
}

void ThumbnailsWidget::setActiveImages(const QStringList &images)
{
  const QSignalBlocker blocker(mListWidget);
  QListWidgetItem *item = nullptr;
  for (int i = 0; i < mListWidget->count(); i++){
    item = mListWidget->item(i);
    item->setSelected(false);
    for (auto &image : images){
      if (item->toolTip().compare(image) == 0){
        item->setSelected(true);
        break;
      }
    }
  }
}

/* public slots */

void ThumbnailsWidget::addThumbnail(const QString &thumb)
{
  QImageReader imageReader(thumb);
  QSize size = imageReader.size();
  double scale = 1.;
  int w = size.width();
  int h = size.height();
  if (w > h){
    scale = w / 200.;
  } else {
    scale = h / 200.;
  }
  size /= scale;

  QFileInfo fileInfo(thumb);

  QPixmap pixmap(size.width(), size.height());
  pixmap.fill(QColor(Qt::GlobalColor::lightGray));
  QIcon icon(pixmap);
  QListWidgetItem *item = new QListWidgetItem(icon, fileInfo.fileName());
  item->setToolTip(fileInfo.absoluteFilePath());
  mListWidget->addItem(item);

  mFutureWatcherThumbnail->setFuture(QtConcurrent::mapped(thumb, makeThumbnail));
}

void ThumbnailsWidget::addThumbnails(const QStringList &thumbs)
{  
  for (auto thumb : thumbs) {

    QImageReader imageReader(thumb);
    QSize size = imageReader.size();
    double scale = 1.;
    int w = size.width();
    int h = size.height();
    if (w > h){
      scale = w / 200.;
    } else {
      scale = h / 200.;
    }
    size /= scale;

    QFileInfo fileInfo(thumb);
    QPixmap pixmap(size.width(), size.height());
    pixmap.fill(QColor(Qt::GlobalColor::lightGray));
    QIcon icon(pixmap);
    QListWidgetItem *item = new QListWidgetItem(icon, fileInfo.fileName());
    item->setToolTip(fileInfo.absoluteFilePath());
    mListWidget->addItem(item);
  }

  if (thumbs.empty() == false) {
    mFutureWatcherThumbnail->setFuture(QtConcurrent::mapped(thumbs, /*&ThumbnailsWidget::*/makeThumbnail));
  }
}

void ThumbnailsWidget::deleteThumbnail(const QString &thumb)
{
  //mListWidget
  QListWidgetItem *item = nullptr;

  for (int i = 0; i < mListWidget->count(); i++){
   item = mListWidget->item(i);
   if (item->toolTip().compare(thumb) == 0) {
     delete item;
     item = nullptr;
     break;
   }
  }
}

void ThumbnailsWidget::onThumbnailDoubleClicked(QListWidgetItem *item)
{
  emit openImage(item->toolTip());
}

void ThumbnailsWidget::onSelectionChanged()
{
  if (mListWidget->currentItem()){
    QList<QListWidgetItem*> item = mListWidget->selectedItems();
    int size = item.size();
    if (size == 1) {
      emit selectImage(item[0]->toolTip());
    } else {
      QStringList selected_images;
      for (int i = 0; i < size; i++){
        selected_images.push_back(item[i]->toolTip());
      }
      emit selectImages(selected_images);
    }
  }
  update();
}

void ThumbnailsWidget::onThumbnailClicked()
{
  mIconSize  = QSize(200, 200);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::IconMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(true);
  mThumbnailSmallAction->setChecked(false);
  mDetailsAction->setChecked(false);
}

void ThumbnailsWidget::onThumbnailSmallClicked()
{
  mIconSize  = QSize(100, 100);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::IconMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(false);
  mThumbnailSmallAction->setChecked(true);
  mDetailsAction->setChecked(false);
}

void ThumbnailsWidget::onDetailsClicked()
{
  mIconSize  = QSize(50, 50);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::ListMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(false);
  mThumbnailSmallAction->setChecked(false);
  mDetailsAction->setChecked(true);
}

void ThumbnailsWidget::onDeleteImageClicked()
{
  if (mListWidget->selectedItems().size() > 0){
    QStringList selectImages;
    for (const auto &item : mListWidget->selectedItems()){
      selectImages.push_back(item->toolTip());
    }
    emit deleteImages(selectImages);
  }
  mThumbnaislSize = mListWidget->count();
}

void ThumbnailsWidget::showThumbnail(int id)
{
  QListWidgetItem *item = mListWidget->item(mThumbnaislSize+id);
  QPixmap pixmap = QPixmap::fromImage(mFutureWatcherThumbnail->resultAt(id));
  QIcon icon(pixmap);
  item->setIcon(icon);
}

void ThumbnailsWidget::finished()
{
  mThumbnaislSize = mListWidget->count();
  emit imagesLoaded();
}

void ThumbnailsWidget::update()
{
  bool images_added = mListWidget->selectedItems().size() > 0;
  mThumbnailAction->setEnabled(images_added);
  mThumbnailSmallAction->setEnabled(images_added);
  mDetailsAction->setEnabled(images_added);
  mDeleteImageAction->setEnabled(images_added);
}

void ThumbnailsWidget::retranslate()
{
  mThumbnailAction->setText(QApplication::translate("ThumbnailsWidget", "Thumbnails"));
  mThumbnailAction->setStatusTip(QApplication::translate("ThumbnailsWidget", "Thumbnail"));
  mThumbnailSmallAction->setText(QApplication::translate("ThumbnailsWidget", "Small Thumbnails"));
  mThumbnailSmallAction->setStatusTip(QApplication::translate("ThumbnailsWidget", "Thumbnail small"));
  mDetailsAction->setText(QApplication::translate("ThumbnailsWidget", "Details"));
  mDetailsAction->setStatusTip(QApplication::translate("ThumbnailsWidget", "Details"));
  mDeleteImageAction->setText(QApplication::translate("ThumbnailsWidget", "Delete image"));
  mDeleteImageAction->setStatusTip(QApplication::translate("ThumbnailsWidget", "Delete image"));
}

void ThumbnailsWidget::clear()
{
  if (mFutureWatcherThumbnail->isRunning()) {
    mFutureWatcherThumbnail->cancel();
  }
  mListWidget->clear();
  mThumbnaislSize = 0;
}

/* Private */

void ThumbnailsWidget::initUI()
{
  this->setWindowTitle("Thumbnails");

  QToolBar *toolBar = new QToolBar(this);

  mThumbnailAction = new QAction(this);

  mThumbnailAction->setIcon(QIcon(":/ico/48/img/material/48/icons8-medium-icons.png"));
  mThumbnailAction->setCheckable(true);
  toolBar->addAction(mThumbnailAction);

  mThumbnailSmallAction = new QAction(this);
  mThumbnailSmallAction->setIcon(QIcon(":/ico/48/img/material/48/icons8-small-icons.png"));
  mThumbnailSmallAction->setCheckable(true);
  toolBar->addAction(mThumbnailSmallAction);

  toolBar->addSeparator();

  mDetailsAction = new QAction(this);
  mDetailsAction->setIcon(QIcon(":/ico/48/img/material/48/icons8-details.png"));
  mDetailsAction->setCheckable(true);
  toolBar->addAction(mDetailsAction);

  toolBar->addSeparator();

  mDeleteImageAction = new QAction(this);
  mDeleteImageAction->setIcon(QIcon(":/ico/48/img/material/48/icons8-close-button.png"));
  toolBar->addAction(mDeleteImageAction);

  onThumbnailClicked();

  mListWidget->setSelectionMode(QListWidget::ExtendedSelection);

  mGridLayout->setMargin(0);
  mGridLayout->addWidget(toolBar);
  mGridLayout->addWidget(mListWidget);

  mFutureWatcherThumbnail = new QFutureWatcher<QImage>(this);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void ThumbnailsWidget::initSignalAndSlots()
{
  connect(mListWidget,             SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onThumbnailDoubleClicked(QListWidgetItem*)));
  connect(mListWidget,             SIGNAL(itemSelectionChanged()),              this, SLOT(onSelectionChanged()));
  connect(mThumbnailAction,        SIGNAL(changed()),                           this, SLOT(onThumbnailClicked()));
  connect(mThumbnailSmallAction,   SIGNAL(changed()),                           this, SLOT(onThumbnailSmallClicked()));
  connect(mDetailsAction,          SIGNAL(changed()),                           this, SLOT(onDetailsClicked()));
  connect(mDeleteImageAction,      SIGNAL(triggered(bool)),                     this, SLOT(onDeleteImageClicked()));
  connect(mFutureWatcherThumbnail, SIGNAL(resultReadyAt(int)),                  this, SLOT(showThumbnail(int)));
  connect(mFutureWatcherThumbnail, SIGNAL(finished()),                          this, SLOT(finished()));
}

void ThumbnailsWidget::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  switch (event->type()) {
    case QEvent::LanguageChange:
      this->retranslate();
      break;
    default:
      break;
  }
}

} // namespace inspector
