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

#include "FeaturesViewerView.h"

#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/GraphicItem.h"
#include "graphos/core/utils.h"

#include <tidop/img/imgreader.h>

#include <QGraphicsItem>
#include <QComboBox>
#include <QTreeWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QApplication>

namespace graphos
{

FeaturesViewerViewImp::FeaturesViewerViewImp(QWidget *parent,
                                             Qt::WindowFlags f)
  : FeaturesViewerView(parent, f),
    mMarkerColor("#e5097e"),
    mSelectedMarkerColor("#ff0000"),
    mMarkerType(1),
    mMarkerSize(20),
    mMarkerWidth(1),
    mSelectedMarkerWidth(2)
{
    this->FeaturesViewerViewImp::initUI();
    this->FeaturesViewerViewImp::initSignalAndSlots();
}

FeaturesViewerViewImp::~FeaturesViewerViewImp()
{
}

void FeaturesViewerViewImp::initUI()
{
    this->setObjectName(QString("FeaturesViewerView"));

    this->resize(994, 688);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelImages = new QLabel(this);
    grid_layout->addWidget(mLabelImages, 0, 0, 1, 1);

    mComboBoxImages = new QComboBox(this);
    grid_layout->addWidget(mComboBoxImages, 0, 1, 1, 1);

    mGraphicView = new GraphicViewer(this);
    grid_layout->addWidget(mGraphicView, 1, 0, 1, 2);

    mTreeWidget = new QTreeWidget(this);
    mTreeWidget->setAlternatingRowColors(true);
    mTreeWidget->setMaximumSize(QSize(16777215, 192));
    grid_layout->addWidget(mTreeWidget, 2, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Close | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 3, 0, 1, 2);

    this->retranslate();
    this->update();
}

void FeaturesViewerViewImp::initSignalAndSlots()
{

    connect(mComboBoxImages, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int idx) {
                emit image_changed(mComboBoxImages->itemData(idx).toULongLong());
            });

    connect(mTreeWidget, &QTreeWidget::itemSelectionChanged,
            this, &FeaturesViewerViewImp::onTreeWidgetItemSelectionChanged);
    connect(mGraphicView, &GraphicViewer::selectionChanged,
            this, &FeaturesViewerViewImp::onGraphicsViewSelectionChanged);

    connect(mButtonBox->button(QDialogButtonBox::Close),
            &QAbstractButton::clicked,
            this,
            &QDialog::close);
    connect(mButtonBox->button(QDialogButtonBox::Help),
            &QAbstractButton::clicked,
            this,
            &DialogView::help);
}

void FeaturesViewerViewImp::clear()
{
    this->setWindowTitle(tr("Keypoints Viewer"));
    const QSignalBlocker blocker_tree_widget(mTreeWidget);
    mTreeWidget->clear();

    const QSignalBlocker blocker_combo_box_images(mComboBoxImages);
    mComboBoxImages->clear();
}

void FeaturesViewerViewImp::update()
{

}

void FeaturesViewerViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("FeaturesViewerView", "Keypoints Viewer"));
    mLabelImages->setText(QApplication::translate("FeaturesViewerView", "Images:"));
    QTreeWidgetItem *tree_widget_item = mTreeWidget->headerItem();
    tree_widget_item->setText(0, QApplication::translate("FeaturesViewerView", "ID"));
    tree_widget_item->setText(1, QApplication::translate("FeaturesViewerView", "X"));
    tree_widget_item->setText(2, QApplication::translate("FeaturesViewerView", "Y"));
    tree_widget_item->setText(3, QApplication::translate("FeaturesViewerView", "Size"));
    tree_widget_item->setText(4, QApplication::translate("FeaturesViewerView", "Angle"));
    mButtonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    mButtonBox->button(QDialogButtonBox::Help)->setText("Help");
}

void FeaturesViewerViewImp::onGraphicsViewSelectionChanged()
{
    const QSignalBlocker blocker1(mGraphicView);
    const QSignalBlocker blocker2(mTreeWidget);

    QList<QGraphicsItem *> items = mGraphicView->items();
    bool selected_item = false;
    for (const auto &item : items) {
        if (item && item->isSelected() == true) {

            mTreeWidget->selectionModel()->clearSelection();

            for (int j = 0; j < mTreeWidget->topLevelItemCount(); j++) {
                QTreeWidgetItem *tree_item = mTreeWidget->topLevelItem(j);
                if (tree_item && tree_item->text(0).compare(item->toolTip()) == 0) {
                    tree_item->setSelected(true);
                    QModelIndex index = mTreeWidget->model()->index(j, 0);
                    mTreeWidget->scrollTo(index);
                    break;
                }
            }

            selected_item = true;
            break;
        }
    }

    if (selected_item == false) {
        for (auto& item : items) {
            item->setSelected(false);
        }
        mTreeWidget->selectionModel()->clearSelection();
    }

    update();
}

void FeaturesViewerViewImp::onTreeWidgetItemSelectionChanged()
{
    const QSignalBlocker blocker1(mGraphicView);

    if (QTreeWidgetItem *item = mTreeWidget->currentItem()) {

        QPointF point(item->text(1).toDouble(), item->text(2).toDouble());
        mGraphicView->scene()->clearSelection();

        mGraphicView->zoom11();
        mGraphicView->centerOn(point);
        QPoint pt_scene = mGraphicView->mapFromScene(point);
        QGraphicsItem *select_item = mGraphicView->itemAt(pt_scene);
        if (select_item) {
            select_item->setSelected(true);
        }

    } else {
        mGraphicView->scene()->clearSelection();
    }

    update();
}

void FeaturesViewerViewImp::setImageList(const std::vector<std::pair<size_t, QString>> &imageList)
{
    QSignalBlocker blocker(mComboBoxImages);
    mComboBoxImages->clear();
    for (auto &image : imageList) {
        QFileInfo file_info(image.second);
        mComboBoxImages->addItem(file_info.baseName(), static_cast<qulonglong>(image.first));
    }
}

void FeaturesViewerViewImp::setCurrentImage(const QString &imagePath)
{
    QSignalBlocker blocker(mComboBoxImages);
    QFileInfo file_info(imagePath);
    mComboBoxImages->setCurrentText(file_info.baseName());
    mGraphicView->scene()->clearSelection();

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(imagePath.toStdString());
    imageReader->open();
    if (imageReader->isOpen()) {
        cv::Mat bmp = imageReader->read();
        mGraphicView->setImage(cvMatToQImage(bmp));
        imageReader->close();
    }
    mGraphicView->zoomExtend();
}

void FeaturesViewerViewImp::setKeyPoints(const std::vector<std::tuple<QPointF, float, float>> &keyPoints)
{
    const QSignalBlocker blocker(mTreeWidget);

    mTreeWidget->clear();

    for (auto &item : mGraphicView->scene()->items()) {
        if (mMarkerType == 0) {
            if (auto key_point = dynamic_cast<KeyPointGraphicsItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 3) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }


    //QBrush brush(Qt::NoBrush);

    //mMarkerSize;
    QPen pen(QColor(mMarkerColor), mMarkerWidth);
    pen.setCosmetic(true);
    QPen select_pen(QColor(mSelectedMarkerColor), mSelectedMarkerWidth);
    select_pen.setCosmetic(true);

    for (size_t i = 0; i < keyPoints.size(); i++) {
        auto tree_widget_item = new QTreeWidgetItem();
        tree_widget_item->setText(0, QString::number(i + 1));
        QPointF point;
        double size;
        double angle;
        std::tie(point, size, angle) = keyPoints[i];

        //QPointF point = keyPoints[i];
        tree_widget_item->setText(1, QString::number(point.x()));
        tree_widget_item->setText(2, QString::number(point.y()));
        tree_widget_item->setText(3, QString::number(size));
        tree_widget_item->setText(4, QString::number(angle));
        mTreeWidget->addTopLevelItem(tree_widget_item);

        if (mMarkerType == 0) {
            auto item = new KeyPointGraphicsItem(point, size, angle);
            item->setPen(pen);
            item->setSelectedPen(select_pen);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item->setToolTip(QString::number(static_cast<int>(i + 1)));
            mGraphicView->scene()->addItem(item);
        } else if (mMarkerType == 1) {
            // Circle
            auto item = new CircleGraphicItem(point, mMarkerSize);
            item->setPen(pen);
            item->setSelectedPen(select_pen);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item->setToolTip(QString::number(static_cast<int>(i + 1)));
            mGraphicView->scene()->addItem(item);
        } else if (mMarkerType == 2) {
            // Cross
            auto item = new CrossGraphicItem(point, mMarkerSize);
            item->setPen(pen);
            item->setSelectedPen(select_pen);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item->setToolTip(QString::number(static_cast<int>(i + 1)));
            mGraphicView->scene()->addItem(item);
        } else if (mMarkerType == 3) {
            // Diagonal cross
            auto item = new DiagonalCrossGraphicItem(point, mMarkerSize);
            item->setPen(pen);
            item->setSelectedPen(select_pen);
            item->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item->setToolTip(QString::number(static_cast<int>(i + 1)));
            mGraphicView->scene()->addItem(item);
        }

    }

}

void FeaturesViewerViewImp::setBackgroundColor(const QString &bgColor)
{
    mGraphicView->setBackgroundBrush(QBrush(QColor(bgColor)));
}

void FeaturesViewerViewImp::setSelectedMarkerStyle(const QString &color, int width)
{
    mSelectedMarkerColor = color;
    mSelectedMarkerWidth = width;
}

void FeaturesViewerViewImp::setMarkerStyle(const QString &color, int width, int type, int size)
{
    if (mMarkerType != type) {  /// TODO: ¿Es necesario?
        for (auto &item : mGraphicView->scene()->items()) {
            if (mMarkerType == 0) {
                if (auto key_point = dynamic_cast<KeyPointGraphicsItem *>(item)) {
                    mGraphicView->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (mMarkerType == 1) {
                // Circle
                if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                    mGraphicView->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (mMarkerType == 2) {
                // Cross
                if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                    mGraphicView->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (mMarkerType == 3) {
                // Diagonal cross
                if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    mGraphicView->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            }
        }
    }

    mMarkerColor = color;
    mMarkerSize = size;
    mMarkerWidth = width;
    mMarkerType = type;

    QPen pen(QColor(mMarkerColor), mMarkerWidth);

    /// Para volver a cargar los puntos
    if (mComboBoxImages->currentText().isEmpty() == false)
        emit image_changed(mComboBoxImages->currentData().toULongLong());

}

void FeaturesViewerViewImp::closeEvent(QCloseEvent *event)
{
    for (auto &item : mGraphicView->scene()->items()) {
        if (mMarkerType == 0) {
            if (auto key_point = dynamic_cast<KeyPointGraphicsItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 3) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicView->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }

    QDialog::closeEvent(event);
}

} // namespace graphos
