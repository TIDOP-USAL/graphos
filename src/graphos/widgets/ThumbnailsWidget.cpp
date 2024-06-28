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
#include "graphos/core/AppStatus.h"

#include <tidop/core/utils.h>
#include <tidop/core/progress.h>
#include <tidop/img/imgreader.h>

TL_DISABLE_WARNINGS
#include <QFileInfo>
#include <QListWidget>
#include <QGridLayout>
#include <QToolBar>
#include <QtConcurrent/QtConcurrentMap>
#include <QImage>
#include <QApplication>
#include <QScrollBar>
TL_DEFAULT_WARNINGS

#include <mutex>

static std::mutex sMutexThumbnail;

namespace graphos
{


LoadThumbnailTask::LoadThumbnailTask(QListWidgetItem *item,
                                     QListWidget *listWidget)
  : mItem(item),
    mListWidget(listWidget)
{
}

LoadThumbnailTask::~LoadThumbnailTask()
{
}


void LoadThumbnailTask::execute(tl::Progress *progressBar)
{
    if (mItem == nullptr) return;

    try {

        const QString thumb = mItem->toolTip();
        std::string image_file = thumb.toStdString();

        auto image_reader = tl::ImageReaderFactory::create(image_file);
        image_reader->open();
        if (image_reader->isOpen()) {
            int w = image_reader->cols();
            int h = image_reader->rows();
            double scale = 1.;
            if (w > h) {
                scale = 200. / static_cast<double>(w);
            } else {
                scale = 200. / static_cast<double>(h);
            }

            cv::Mat bmp = image_reader->read(scale, scale);

            QImage image = graphos::cvMatToQImage(bmp);

            image_reader->close();

            QPixmap pixmap = QPixmap::fromImage(image);
            QIcon icon(pixmap);
            mItem->setIcon(icon);

            mListWidget->viewport()->update();

        } else {
            tl::Message::error("Error al abrir la imagen: {}", image_file);
        }

    } catch (const std::exception &e) {
        tl::printException(e);
    }
}





ThumbnailsWidget::ThumbnailsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mListWidget(new QListWidget(this)),
    mGridLayout(new QGridLayout(this)),
    mThumbnailAction(nullptr),
    mDetailsAction(nullptr),
    mDeleteImageAction(nullptr),
    mThumbnaislSize(0),
    bLoadingImages(false),
    mThumbLoad(new tl::TaskQueue),
    mLoadImages(true)
{
    ThumbnailsWidget::initUI();
    ThumbnailsWidget::initSignalAndSlots();
}

ThumbnailsWidget::~ThumbnailsWidget()
{
    if (mThumbLoad) {

        // Para evitar una excepción al cerrar la aplicación si se esta ejecutando
        while (!mThumbLoad->empty());

        delete mThumbLoad;
        mThumbLoad = nullptr;
    }
}

void ThumbnailsWidget::setActiveImage(size_t imageId)
{
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < mListWidget->count(); i++) {
        item = mListWidget->item(i);
        size_t id = item->data(Qt::UserRole).toULongLong();
        item->setSelected(id == imageId);
    }
}

void ThumbnailsWidget::setActiveImages(const std::vector<size_t> &imageIds)
{
    const QSignalBlocker blocker(mListWidget);
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < mListWidget->count(); i++) {
        item = mListWidget->item(i);
        item->setSelected(false);
        for (auto id : imageIds) {
            if (item->data(Qt::UserRole).toULongLong() == id) {
                item->setSelected(item->data(Qt::UserRole).toULongLong() == id);
                break;
            }
        }
    }
}

/* public slots */

void ThumbnailsWidget::addThumbnail(const Image &image, const QSize &imageSize)
{
    std::lock_guard<std::mutex> lck(sMutexThumbnail);

    bLoadingImages = true;

    try {

        QSize size = imageSize;
        int w = size.width();
        int h = size.height();
        double scale = 1.;
        if (size.width() > size.height()) {
            scale = 200. / static_cast<double>(size.width());
        } else {
            scale = 200. / static_cast<double>(size.height());
        }

        size *= scale;
        QPixmap pixmap(size.width(), size.height());
        pixmap.fill(QColor(Qt::GlobalColor::lightGray));
        QIcon icon(pixmap);
        QListWidgetItem *item = new QListWidgetItem(icon, image.name());
        item->setToolTip(image.path());
        item->setData(Qt::UserRole, static_cast<qulonglong>(image.id()));
        mListWidget->addItem(item);
        // TODO: Utilizar setGridSize para ajustar el tamaño
        //mListWidget->setGridSize();
        if (mLoadImages)
            loadVisibleImages();

    } catch (const std::exception &e) {
        tl::printException(e);
    }

    mThumbnaislSize = mListWidget->count();
    bLoadingImages = false;

    update();
}

void ThumbnailsWidget::deleteImages(const std::vector<size_t> &imageIds)
{
    QListWidgetItem *item = nullptr;

    for (int i = 0; i < mListWidget->count(); i++) {
        item = mListWidget->item(i);
        for (auto image_id : imageIds) {
            if (item && item->data(Qt::UserRole).toULongLong() == image_id) {
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
    if (mListWidget->currentItem()) {
        QList<QListWidgetItem *> item = mListWidget->selectedItems();
        int size = item.size();
        if (size == 1) {
            emit select_image(item[0]->data(Qt::UserRole).toULongLong());
        } else {
            std::vector<size_t> selected_images;
            selected_images.reserve(size);
            for (int i = 0; i < size; i++) {
                selected_images.push_back(item[i]->data(Qt::UserRole).toULongLong());
            }
            emit select_images(selected_images);
        }
    }
    update();
}

void ThumbnailsWidget::loadVisibleImages()
{
    if (mListWidget->count() == 0) return;

    QRect rect = mListWidget->viewport()->rect();
    QRegion region = mListWidget->viewport()->visibleRegion();

    mLoadImages = false;

    for (int i = 0; i < mListWidget->count(); i++) {
        QModelIndex idx = mListWidget->model()->index(i, 0);
        QRect idx_rect = mListWidget->visualRect(idx);
        if (region.contains(idx_rect) || region.intersects(idx_rect)) {
            auto item = mListWidget->item(i);
            if (!item->data(Qt::UserRole + 1).toBool()) {

                mLoadImages = true;

                item->setData(Qt::UserRole + 1, true);

                mThumbLoad->push(std::make_shared<LoadThumbnailTask>(item, mListWidget));

            }

        }
    }

    mThumbLoad->runAsync();
}

void ThumbnailsWidget::onThumbnailClicked()
{
    mIconSize = QSize(200, 200);
    mListWidget->setIconSize(mIconSize);
    mListWidget->setViewMode(QListWidget::IconMode);
    mListWidget->setResizeMode(QListWidget::Adjust);
    //mListWidget->setGridSize(QSize(210, 200));
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
    mIconSize = QSize(100, 100);
    mListWidget->setIconSize(mIconSize);
    mListWidget->setViewMode(QListWidget::IconMode);
    mListWidget->setResizeMode(QListWidget::Adjust);
    mListWidget->verticalScrollBar()->setSingleStep(100);
    //mListWidget->setGridSize(QSize(110, 100));
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
    mIconSize = QSize(50, 50);
    mListWidget->setIconSize(mIconSize);
    mListWidget->setViewMode(QListWidget::ListMode);
    mListWidget->setResizeMode(QListWidget::Adjust);
    //mListWidget->setGridSize(QSize(-1, -1));
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

    if (!mListWidget->selectedItems().empty()) {
        std::vector<size_t> selectImages;
        for (const auto &item : mListWidget->selectedItems()) {
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
    mDeleteImageAction->setEnabled(!mListWidget->selectedItems().empty() && !bLoadingImages);
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
    if (!mThumbLoad->empty()) {
        mThumbLoad->stop();
        while (mThumbLoad->status() == tl::TaskQueue::Status::stopping) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
    }
    mListWidget->clear();
    mThumbnaislSize = 0;
    bLoadingImages = false;
    mLoadImages = true;

    update();
}

/* Private */

void ThumbnailsWidget::initUI()
{
    setWindowTitle("Thumbnails");

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
    mListWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    mGridLayout->setMargin(0);
    mGridLayout->addWidget(toolBar);
    mGridLayout->addWidget(mListWidget);

    ThumbnailsWidget::retranslate();
    ThumbnailsWidget::clear(); /// set default values
}

void ThumbnailsWidget::initSignalAndSlots()
{
    connect(mListWidget, &QListWidget::itemDoubleClicked, this, &ThumbnailsWidget::onThumbnailDoubleClicked);
    connect(mListWidget, &QListWidget::itemSelectionChanged, this, &ThumbnailsWidget::onSelectionChanged);
    connect(mThumbnailAction, &QAction::changed, this, &ThumbnailsWidget::onThumbnailClicked);
    connect(mThumbnailSmallAction, &QAction::changed, this, &ThumbnailsWidget::onThumbnailSmallClicked);
    connect(mDetailsAction, &QAction::changed, this, &ThumbnailsWidget::onDetailsClicked);
    connect(mDeleteImageAction, &QAction::triggered, this, &ThumbnailsWidget::onDeleteImageClicked);
    connect(mListWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, &ThumbnailsWidget::loadVisibleImages);
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
