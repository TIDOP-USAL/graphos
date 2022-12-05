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

#include "ThumbnailsWidget.h"

#include "graphos/core/utils.h"
#include "graphos/core/Application.h"

#include <tidop/core/utils.h>
#include <tidop/img/imgreader.h>

#include <QFileInfo>
#include <QListWidget>
#include <QGridLayout>
#include <QToolBar>
#include <QtConcurrent/QtConcurrentMap>
#include <QImage>
#include <QApplication>
#include <QScrollBar>

#include <mutex>

static std::mutex sMutexThumbnail;



void makeThumbnail(QListWidgetItem *item, QListWidget *listWidget)
{
  std::lock_guard<std::mutex> lck(sMutexThumbnail);

  if (item == nullptr) return;

  QImage image;

  try {

    const QString thumb = item->toolTip();
    std::string image_file = thumb.toStdString();

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(image_file);
    imageReader->open();
    if (imageReader->isOpen()) {

      int w = imageReader->cols();
      int h = imageReader->rows();
      double scale = 1.;
      if (w > h) {
        scale = 200. / static_cast<double>(w);
      } else {
        scale = 200. / static_cast<double>(h);
      }

      cv::Mat bmp = imageReader->read(scale, scale);

      image = graphos::cvMatToQImage(bmp);

      imageReader->close();

      QPixmap pixmap = QPixmap::fromImage(image);
      QIcon icon(pixmap);
      item->setIcon(icon);

      listWidget->viewport()->update();

    } else {
      msgError("Error al abrir la imagen: %s", image_file.c_str());
    }

  } catch (const std::exception &e) {
    msgError(e.what());
  }

  //return image;
}

namespace graphos
{

ThumbnailsWidget::ThumbnailsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mListWidget(new QListWidget(this)),
    mGridLayout(new QGridLayout(this)),
    mThumbnailAction(nullptr),
    mDetailsAction(nullptr),
    mDeleteImageAction(nullptr),
    mThumbnaislSize(0),
    bLoadingImages(false)
{
  ThumbnailsWidget::initUI();
  ThumbnailsWidget::initSignalAndSlots();	
}

void ThumbnailsWidget::setActiveImage(size_t imageId)
{
  QListWidgetItem *item = nullptr;
  for (int i = 0; i < mListWidget->count(); i++){
   item = mListWidget->item(i);
   size_t id = item->data(Qt::UserRole).toULongLong();
   item->setSelected(id == imageId);
  }
}

void ThumbnailsWidget::setActiveImages(const std::vector<size_t> &imageIds)
{
  const QSignalBlocker blocker(mListWidget);
  QListWidgetItem *item = nullptr;
  for (int i = 0; i < mListWidget->count(); i++){
    item = mListWidget->item(i);
    item->setSelected(false);
    for (auto id : imageIds){
      if (item->data(Qt::UserRole).toULongLong() == id) {
        item->setSelected(item->data(Qt::UserRole).toULongLong() == id);
        break;
      }
    }
  }
}

/* public slots */

void ThumbnailsWidget::addThumbnail(const QString &thumb, size_t imageId)
{

  std::lock_guard<std::mutex> lck(sMutexThumbnail);

  bLoadingImages = true;
  QImage image;

  try {

    std::string image_file = thumb.toStdString();

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(image_file);
    imageReader->open();
    if (imageReader->isOpen()) {
      
      int w = imageReader->cols();
      int h = imageReader->rows();
      double scale = 1.;
      if (w > h) {
        scale =  200. / static_cast<double>(w);
      } else {
        scale = 200. / static_cast<double>(h);
      }

      imageReader->close();

      QSize size(w, h);
      size *= scale;
      QPixmap pixmap(size.width(), size.height());
      pixmap.fill(QColor(Qt::GlobalColor::lightGray));									  
      QIcon icon(pixmap);
      QFileInfo fileInfo(thumb);
      QListWidgetItem *item = new QListWidgetItem(icon, fileInfo.baseName());
      item->setToolTip(fileInfo.absoluteFilePath());
      item->setData(Qt::UserRole, imageId);
      mListWidget->addItem(item);

      loadVisibleImages();

    } else {
      msgError("Error al abrir la imagen: %s", image_file.c_str());
    }

  } catch (const std::exception &e) {
    msgError(e.what());
  }

  mThumbnaislSize = mListWidget->count();
  bLoadingImages = false;
    
  update();
}

void ThumbnailsWidget::addThumbnails(const QStringList &thumbs)
{  
  bLoadingImages = true;

  for (auto &thumb : thumbs) {

    try {

      std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(thumb.toStdString());
      imageReader->open();
      if (imageReader->isOpen()) {
      
        int w = imageReader->cols();
        int h = imageReader->rows();
        QSize size(w, h);
        double scale = 1.;
        if (w > h) {
          scale = static_cast<double>(w) / 200.;
        } else {
          scale = static_cast<double>(h) / 200.;
        }
        size /= scale;

        imageReader->close();

        QFileInfo fileInfo(thumb);
        QPixmap pixmap(size.width(), size.height());
        pixmap.fill(QColor(Qt::GlobalColor::lightGray));
        QIcon icon(pixmap);
        QListWidgetItem *item = new QListWidgetItem(icon, fileInfo.baseName());
        item->setToolTip(fileInfo.absoluteFilePath());
        mListWidget->addItem(item);

      } else {
        msgError("Error al abrir la imagen: %s", thumb.toStdString().c_str());
      }
    } catch (std::exception &e) {
      tl::printException(e);
    }
  }

  loadVisibleImages();

  bLoadingImages = false;

  update();
}

void ThumbnailsWidget::deleteImages(const std::vector<size_t> &imageIds)
{
  QListWidgetItem *item = nullptr;

  for (int i = 0; i < mListWidget->count(); i++) {
    item = mListWidget->item(i);
    for (auto imageId : imageIds) {
      if (item && item->data(Qt::UserRole).toULongLong() == imageId) {
        delete item;
        item = nullptr;
        i--;
        break;
      }
    }
  }

  update();
}

void ThumbnailsWidget::onThumbnailDoubleClicked(QListWidgetItem *item)
{
  emit open_image(item->data(Qt::UserRole).toULongLong());
}

void ThumbnailsWidget::onSelectionChanged()
{
  if (mListWidget->currentItem()){
    QList<QListWidgetItem*> item = mListWidget->selectedItems();
    int size = item.size();
    if (size == 1) {
      emit select_image(item[0]->data(Qt::UserRole).toULongLong());
    } else {
      std::vector<size_t> selected_images;
      for (int i = 0; i < size; i++){
        selected_images.push_back(item[i]->data(Qt::UserRole).toULongLong());
      }
      emit select_images(selected_images);
    }
  }
  update();
}

void ThumbnailsWidget::loadVisibleImages()
{
  Application &app = Application::instance();
  AppStatus *app_status = app.status();
  TL_ASSERT(app_status != nullptr, "AppStatus is null");

  QRect rect = mListWidget->viewport()->rect();
  QRegion region = mListWidget->viewport()->visibleRegion();

  for (int i = 0; i < mListWidget->count(); i++) {
    QModelIndex idx = mListWidget->model()->index(i, 0);
    QRect idx_rect = mListWidget->visualRect(idx);
    if (region.contains(idx_rect) || region.intersects(idx_rect)) {
      auto item = mListWidget->item(i);
      if (!item->data(Qt::UserRole + 1).toBool()) {

        item->setData(Qt::UserRole + 1, true);

        std::thread t(makeThumbnail, item, mListWidget);
        t.detach();
      }

    }
  }
}

void ThumbnailsWidget::onThumbnailClicked()
{
  mIconSize  = QSize(200, 200);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::IconMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  mListWidget->verticalScrollBar()->setSingleStep(175); 
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(true);
  mThumbnailSmallAction->setChecked(false);
  mDetailsAction->setChecked(false);
  loadVisibleImages();
}

void ThumbnailsWidget::onThumbnailSmallClicked()
{
  mIconSize  = QSize(100, 100);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::IconMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  mListWidget->verticalScrollBar()->setSingleStep(100);
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(false);
  mThumbnailSmallAction->setChecked(true);
  mDetailsAction->setChecked(false);
  loadVisibleImages();
}

void ThumbnailsWidget::onDetailsClicked()
{
  mIconSize  = QSize(50, 50);
  mListWidget->setIconSize(mIconSize);
  mListWidget->setViewMode(QListWidget::ListMode);
  mListWidget->setResizeMode(QListWidget::Adjust);
  mListWidget->verticalScrollBar()->setSingleStep(50);
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);
  mThumbnailAction->setChecked(false);
  mThumbnailSmallAction->setChecked(false);
  mDetailsAction->setChecked(true);
  loadVisibleImages();
}

void ThumbnailsWidget::onDeleteImageClicked()
{
  std::lock_guard<std::mutex> lck(sMutexThumbnail);

  if (mListWidget->selectedItems().size() > 0){
    std::vector<size_t> selectImages;
    for (const auto &item : mListWidget->selectedItems()){
      selectImages.push_back(item->data(Qt::UserRole).toULongLong());
    }
    emit delete_images(selectImages);
  }
  mThumbnaislSize = mListWidget->count();
  loadVisibleImages();
}

void ThumbnailsWidget::update()
{
  const QSignalBlocker block0(mThumbnailAction);
  const QSignalBlocker block1(mThumbnailSmallAction);
  const QSignalBlocker block2(mDetailsAction);

  bool images_added = mListWidget->count() > 0;
  mThumbnailAction->setEnabled(images_added);
  mThumbnailSmallAction->setEnabled(images_added);
  mDetailsAction->setEnabled(images_added);
  mDeleteImageAction->setEnabled(mListWidget->selectedItems().size() > 0 && !bLoadingImages);
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
  mListWidget->clear();
  mThumbnaislSize = 0;
  bLoadingImages = false;

  ThumbnailsWidget::update();
}

/* Private */

void ThumbnailsWidget::initUI()
{
  this->setWindowTitle("Thumbnails");

  QToolBar *toolBar = new QToolBar(this);

  mThumbnailAction = new QAction(this);

  mThumbnailAction->setIcon(QIcon::fromTheme("medium-icons"));
  mThumbnailAction->setCheckable(true);
  toolBar->addAction(mThumbnailAction);

  mThumbnailSmallAction = new QAction(this);
  mThumbnailSmallAction->setIcon(QIcon::fromTheme("small-icons"));
  mThumbnailSmallAction->setCheckable(true);
  toolBar->addAction(mThumbnailSmallAction);

  toolBar->addSeparator();

  mDetailsAction = new QAction(this);
  mDetailsAction->setIcon(QIcon::fromTheme("details"));
  mDetailsAction->setCheckable(true);
  toolBar->addAction(mDetailsAction);

  toolBar->addSeparator();

  mDeleteImageAction = new QAction(this);
  mDeleteImageAction->setIcon(QIcon::fromTheme("remove-image"));
  toolBar->addAction(mDeleteImageAction);

  onThumbnailClicked();

  mListWidget->setSelectionMode(QListWidget::ExtendedSelection);

  mGridLayout->setMargin(0);
  mGridLayout->addWidget(toolBar);
  mGridLayout->addWidget(mListWidget);

  ThumbnailsWidget::retranslate();
  ThumbnailsWidget::clear(); /// set default values
}

void ThumbnailsWidget::initSignalAndSlots()
{
  connect(mListWidget,             &QListWidget::itemDoubleClicked,     this, &ThumbnailsWidget::onThumbnailDoubleClicked);
  connect(mListWidget,             &QListWidget::itemSelectionChanged,  this, &ThumbnailsWidget::onSelectionChanged);
  connect(mThumbnailAction,        &QAction::changed,                   this, &ThumbnailsWidget::onThumbnailClicked);
  connect(mThumbnailSmallAction,   &QAction::changed,                   this, &ThumbnailsWidget::onThumbnailSmallClicked);
  connect(mDetailsAction,          &QAction::changed,                   this, &ThumbnailsWidget::onDetailsClicked);
  connect(mDeleteImageAction,      &QAction::triggered,                 this, &ThumbnailsWidget::onDeleteImageClicked);
  connect(mListWidget->verticalScrollBar(), &QScrollBar::valueChanged,  this, &ThumbnailsWidget::loadVisibleImages);
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

void ThumbnailsWidget::resizeEvent(QResizeEvent *event)
{
  loadVisibleImages();
}

} // namespace graphos
