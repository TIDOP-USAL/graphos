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

#include "MatchViewerView.h"

#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/GraphicItem.h"
#include "graphos/core/utils.h"

#include <tidop/img/imgreader.h>

#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGraphicsItem>
#include <QComboBox>
#include <QTreeWidget>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace graphos
{

MatchViewerViewImp::MatchViewerViewImp(QWidget *parent,
                                       Qt::WindowFlags f)
  : MatchViewerView(parent, f),
    bUnsavedChanges(false),
    mMarkerColor("#e5097e"),
    mMarkerSize(20),
    mMarkerWidth(1),
    mMarkerType(2),
    mSelectedMarkerColor("#ff0000"),
    mSelectedMarkerWidth(2)
{
    MatchViewerViewImp::initUI();
    MatchViewerViewImp::initSignalAndSlots();
}

MatchViewerViewImp::~MatchViewerViewImp()
{
}

void MatchViewerViewImp::initUI()
{
    this->setObjectName(QString("MatchViewerView"));

    this->resize(994, 688);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelLeftImage = new QLabel(this);
    grid_layout->addWidget(mLabelLeftImage, 0, 0, 1, 1);

    mLabelRightImage = new QLabel(this);
    grid_layout->addWidget(mLabelRightImage, 0, 1, 1, 1);

    mComboBoxLeftImage = new QComboBox(this);
    grid_layout->addWidget(mComboBoxLeftImage, 1, 0, 1, 1);

    mComboBoxRightImage = new QComboBox(this);
    grid_layout->addWidget(mComboBoxRightImage, 1, 1, 1, 1);

    mGraphicsViewLeftImage = new GraphicViewer(this);
    grid_layout->addWidget(mGraphicsViewLeftImage, 2, 0, 1, 1);

    mGraphicsViewRightImage = new GraphicViewer(this);
    grid_layout->addWidget(mGraphicsViewRightImage, 2, 1, 1, 1);

    mLabelMatches = new QLabel(this);
    grid_layout->addWidget(mLabelMatches, 3, 0, 1, 1);

    auto grid_layout2 = new QGridLayout();

    mTreeWidgetMatches = new QTreeWidget(this);
    mTreeWidgetMatches->setMaximumSize(QSize(16777215, 285));
    mTreeWidgetMatches->setAlternatingRowColors(true);

    grid_layout2->addWidget(mTreeWidgetMatches, 0, 0, 4, 1);

    mPushButtonDeleteMatch = new QPushButton(this);
    mPushButtonDeleteMatch->setMaximumSize(QSize(280, 16777215));
    grid_layout2->addWidget(mPushButtonDeleteMatch, 0, 1, 1, 1);
    /// Desactivo el borrado de puntos por ahora
    mPushButtonDeleteMatch->setVisible(false);

    grid_layout->addLayout(grid_layout2, 4, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Close | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 5, 0, 1, 2);

    this->retranslate();
    this->update();
}

void MatchViewerViewImp::initSignalAndSlots()
{
    connect(mComboBoxLeftImage,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int idx) {
                emit leftImageChange(mComboBoxLeftImage->itemData(idx).toULongLong());
            });

    connect(mComboBoxRightImage,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int idx) {
                size_t image_right = mComboBoxRightImage->itemData(idx).toULongLong();
                size_t image_left = mComboBoxLeftImage->currentData().toULongLong();
                emit rightImageChange(image_right);
                emit loadMatches(image_left, image_right);
            });

    connect(mTreeWidgetMatches, &QTreeWidget::itemSelectionChanged,
            this, &MatchViewerViewImp::onTreeWidgetMatchesItemSelectionChanged);

    connect(mGraphicsViewLeftImage, &GraphicViewer::selectionChanged,
            this, &MatchViewerViewImp::onGraphicsViewLeftSelectionChanged);

    connect(mGraphicsViewRightImage, &GraphicViewer::selectionChanged,
            this, &MatchViewerViewImp::onGraphicsViewRightSelectionChanged);

    connect(mButtonBox->button(QDialogButtonBox::Close),
            &QAbstractButton::clicked,
            this,
            &QDialog::close);

    connect(mButtonBox->button(QDialogButtonBox::Help),
            &QAbstractButton::clicked,
            this,
            &DialogView::help);
}

void MatchViewerViewImp::clear()
{
    const QSignalBlocker blocker1(mComboBoxLeftImage);
    const QSignalBlocker blocker2(mComboBoxLeftImage);

    mComboBoxLeftImage->clear();
    mComboBoxLeftImage->clear();
}

void MatchViewerViewImp::update()
{
    mPushButtonDeleteMatch->setEnabled(!mTreeWidgetMatches->selectedItems().empty());
}

void MatchViewerViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("MatchViewerView", "Match Viewer"));
    mLabelLeftImage->setText(QApplication::translate("MatchViewerView", "Left Image:"));
    mLabelRightImage->setText(QApplication::translate("MatchViewerView", "Right Image:"));
    mLabelMatches->setText(QApplication::translate("MatchViewerView", "Matches:"));
    QTreeWidgetItem *qTreeWidgetItem = mTreeWidgetMatches->headerItem();
    qTreeWidgetItem->setText(0, "ID");
    qTreeWidgetItem->setText(1, QApplication::translate("MatchViewerView", "Keypoint Left"));
    qTreeWidgetItem->setText(2, QApplication::translate("MatchViewerView", "Left X"));
    qTreeWidgetItem->setText(3, QApplication::translate("MatchViewerView", "Left Y"));
    qTreeWidgetItem->setText(4, QApplication::translate("MatchViewerView", "Keypoint Right"));
    qTreeWidgetItem->setText(5, QApplication::translate("MatchViewerView", "Right X"));
    qTreeWidgetItem->setText(6, QApplication::translate("MatchViewerView", "Right Y"));
    qTreeWidgetItem->setText(7, QApplication::translate("MatchViewerView", "Distance"));
    mPushButtonDeleteMatch->setText(QApplication::translate("MatchViewerView", "Delete Match"));
    mButtonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    mButtonBox->button(QDialogButtonBox::Help)->setText("Help");
}

void MatchViewerViewImp::setLeftImage(const QString &imageLeft)
{
    QSignalBlocker blocker(mComboBoxLeftImage);
    QFileInfo file_info(imageLeft);
    mComboBoxLeftImage->setCurrentText(file_info.baseName());
    mGraphicsViewLeftImage->scene()->clearSelection();

    auto image_reader = tl::ImageReaderFactory::create(imageLeft.toStdString());
    image_reader->open();
    if (image_reader->isOpen()) {
        cv::Mat bmp = image_reader->read();
        mGraphicsViewLeftImage->setImage(cvMatToQImage(bmp));
        image_reader->close();
    }
}

void MatchViewerViewImp::setRightImage(const QString &imageRight)
{
    QSignalBlocker blocker(mComboBoxRightImage);
    QFileInfo file_info(imageRight);
    mComboBoxRightImage->setCurrentText(imageRight);
    mGraphicsViewRightImage->scene()->clearSelection();

    auto image_reader = tl::ImageReaderFactory::create(imageRight.toStdString());
    image_reader->open();
    if (image_reader->isOpen()) {
        cv::Mat bmp = image_reader->read();
        mGraphicsViewRightImage->setImage(cvMatToQImage(bmp));
        image_reader->close();
    }
}

void MatchViewerViewImp::setLeftImageList(const std::vector<std::pair<size_t, QString>> &leftImageList)
{
    QSignalBlocker blocker(mComboBoxLeftImage);
    mComboBoxLeftImage->clear();
    for (auto &image : leftImageList) {
        QFileInfo file_info(image.second);
        mComboBoxLeftImage->addItem(file_info.baseName(), static_cast<qulonglong>(image.first));
    }
}

void MatchViewerViewImp::setRightImageList(const std::vector<std::pair<size_t, QString>> &rightImageList)
{
    QSignalBlocker blocker(mComboBoxRightImage);
    mComboBoxRightImage->clear();
    for (auto &image : rightImageList) {
        QFileInfo file_info(image.second);
        mComboBoxRightImage->addItem(file_info.baseName(), static_cast<qulonglong>(image.first));
    }
}

void MatchViewerViewImp::setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF> > &matches)
{
    QSignalBlocker blocker(mTreeWidgetMatches);
    mTreeWidgetMatches->clear();

    for (auto &item : mGraphicsViewLeftImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }

    for (auto &item : mGraphicsViewRightImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }


    mGraphicsViewLeftImage->zoomExtend();
    mGraphicsViewRightImage->zoomExtend();

    QPen pen(QColor(mMarkerColor), mMarkerWidth);
    pen.setCosmetic(true);
    QPen select_pen(QColor(mSelectedMarkerColor), mSelectedMarkerWidth);
    select_pen.setCosmetic(true);

    for (const auto& match : matches) {

        auto tree_widget_item = new QTreeWidgetItem();
        size_t pp_id, query_id, train_id;
        QPointF query_point, train_point;
        std::tie(pp_id, query_id, query_point, train_id, train_point) = match;
        tree_widget_item->setText(0, QString::number(pp_id));
        tree_widget_item->setText(1, QString::number(query_id));
        tree_widget_item->setText(2, QString::number(query_point.x()));
        tree_widget_item->setText(3, QString::number(query_point.y()));
        tree_widget_item->setText(4, QString::number(train_id));
        tree_widget_item->setText(5, QString::number(train_point.x()));
        tree_widget_item->setText(6, QString::number(train_point.y()));
        mTreeWidgetMatches->addTopLevelItem(tree_widget_item);

        if (mMarkerType == 0) {
            // Circle
            auto item_left = new CircleGraphicItem(query_point, mMarkerSize);
            item_left->setPen(pen);
            item_left->setSelectedPen(select_pen);
            item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item_left->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(item_left);
            auto item_right = new CircleGraphicItem(train_point, mMarkerSize);
            item_right->setPen(pen);
            item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
            item_right->setSelectedPen(select_pen);
            item_right->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(item_right);
        } else if (mMarkerType == 1) {
            // Cross
            auto cross_graphic_item_left = new CrossGraphicItem(query_point, mMarkerSize);
            cross_graphic_item_left->setPen(pen);
            cross_graphic_item_left->setSelectedPen(select_pen);
            cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
            cross_graphic_item_left->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(cross_graphic_item_left);
            auto cross_graphic_item_right = new CrossGraphicItem(train_point, mMarkerSize);
            cross_graphic_item_right->setPen(pen);
            cross_graphic_item_right->setSelectedPen(select_pen);
            cross_graphic_item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
            cross_graphic_item_right->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(cross_graphic_item_right);
        } else if (mMarkerType == 2) {
            // Diagonal cross
            auto cross_graphic_item_left = new DiagonalCrossGraphicItem(query_point, mMarkerSize);
            cross_graphic_item_left->setPen(pen);
            cross_graphic_item_left->setSelectedPen(select_pen);
            cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
            cross_graphic_item_left->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(cross_graphic_item_left);
            auto cross_graphic_item_right = new DiagonalCrossGraphicItem(train_point, mMarkerSize);
            cross_graphic_item_right->setPen(pen);
            cross_graphic_item_right->setSelectedPen(select_pen);
            cross_graphic_item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
            cross_graphic_item_right->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(cross_graphic_item_right);
        }
    }
}

void MatchViewerViewImp::setBackgroundColor(const QString &bgColor)
{
    mGraphicsViewLeftImage->setBackgroundBrush(QBrush(QColor(bgColor)));
    mGraphicsViewRightImage->setBackgroundBrush(QBrush(QColor(bgColor)));
}

void MatchViewerViewImp::setSelectedMarkerStyle(const QString &color, int width)
{
    mSelectedMarkerColor = color;
    mSelectedMarkerWidth = width;
}

void MatchViewerViewImp::setMarkerStyle(const QString &color, int width, int type, int size)
{
    int markerTypeOld = mMarkerType;
    mMarkerColor = color;
    mMarkerSize = size;
    mMarkerWidth = width;
    mMarkerType = type;

    QPen pen(QColor(mMarkerColor), mMarkerWidth);
    pen.setCosmetic(true);
    QPen select_pen(QColor(mSelectedMarkerColor), mSelectedMarkerWidth);
    select_pen.setCosmetic(true);

    if (markerTypeOld != mMarkerType) {

        for (auto &item : mGraphicsViewLeftImage->scene()->items()) {

            QPointF point;
            int id = 0;

            if (markerTypeOld == 0) {
                // Circle
                if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (markerTypeOld == 1) {
                // Cross
                if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (markerTypeOld == 2) {
                // Diagonal cross
                if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    delete key_point;
                    key_point = nullptr;
                }
            } else {
                continue;
            }

            if (mMarkerType == 0) {
                // Circle
                auto item_left = new CircleGraphicItem(point, mMarkerSize);
                item_left->setPen(pen);
                item_left->setSelectedPen(select_pen);
                item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
                item_left->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(item_left);
            } else if (mMarkerType == 1) {
                // Cross
                auto cross_graphic_item_left = new CrossGraphicItem(point, mMarkerSize);
                cross_graphic_item_left->setPen(pen);
                cross_graphic_item_left->setSelectedPen(select_pen);
                cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
                cross_graphic_item_left->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(cross_graphic_item_left);
            } else if (mMarkerType == 2) {
                // Diagonal cross
                auto cross_graphic_item_left = new DiagonalCrossGraphicItem(point, mMarkerSize);
                cross_graphic_item_left->setPen(pen);
                cross_graphic_item_left->setSelectedPen(select_pen);
                cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
                cross_graphic_item_left->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(cross_graphic_item_left);
            }
        }

        for (auto &item : mGraphicsViewRightImage->scene()->items()) {

            QPointF point;
            int id = 0;

            if (markerTypeOld == 0) {
                // Circle
                if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (markerTypeOld == 1) {
                // Cross
                if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete key_point;
                    key_point = nullptr;
                }
            } else if (markerTypeOld == 2) {
                // Diagonal cross
                if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete key_point;
                    key_point = nullptr;
                }
            } else {
                continue;
            }

            if (mMarkerType == 0) {
                // Circle
                auto item_right = new CircleGraphicItem(point, mMarkerSize);
                item_right->setPen(pen);
                item_right->setSelectedPen(select_pen);
                item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
                item_right->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(item_right);
            } else if (mMarkerType == 1) {
                // Cross
                auto cross_graphic_item_right = new CrossGraphicItem(point, mMarkerSize);
                cross_graphic_item_right->setPen(pen);
                cross_graphic_item_right->setSelectedPen(select_pen);
                cross_graphic_item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
                cross_graphic_item_right->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(cross_graphic_item_right);
            } else if (mMarkerType == 2) {
                // Diagonal cross
                auto cross_graphic_item_right = new DiagonalCrossGraphicItem(point, mMarkerSize);
                cross_graphic_item_right->setPen(pen);
                cross_graphic_item_right->setSelectedPen(select_pen);
                cross_graphic_item_right->setFlag(QGraphicsItem::ItemIsSelectable, true);
                cross_graphic_item_right->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(cross_graphic_item_right);
            }
        }

    } else {

        for (auto &item : mGraphicsViewLeftImage->scene()->items()) {
            if (mMarkerType == 0) {
                // Circle
                if (auto key_points = dynamic_cast<CircleGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 1) {
                // Cross
                if (auto key_points = dynamic_cast<CrossGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 2) {
                // Diagonal cross
                if (auto key_points = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            }
        }

        for (auto &item : mGraphicsViewRightImage->scene()->items()) {
            if (mMarkerType == 0) {
                // Circle
                if (auto key_points = dynamic_cast<CircleGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 1) {
                // Cross
                if (auto key_points = dynamic_cast<CrossGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 2) {
                // Diagonal cross
                if (auto key_points = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    key_points->setPen(pen);
                    key_points->setSelectedPen(select_pen);
                    key_points->setSize(mMarkerSize);
                }
            }
        }

    }

}

void MatchViewerViewImp::setLineStyle(const QString &color, int width)
{
    mLineColor = color;
    mLineWidth = width;
}

void MatchViewerViewImp::onTreeWidgetMatchesItemClicked(QTreeWidgetItem *item, int col) const
{
    tl::unusedParameter(col);

    QPointF query_point(item->text(1).toDouble(), item->text(2).toDouble());
    QPointF train_point(item->text(3).toDouble(), item->text(4).toDouble());

    mGraphicsViewLeftImage->zoom11();
    mGraphicsViewLeftImage->centerOn(query_point);
    mGraphicsViewRightImage->zoom11();
    mGraphicsViewRightImage->centerOn(train_point);
}

void MatchViewerViewImp::onTreeWidgetMatchesItemSelectionChanged()
{
    const QSignalBlocker blocker1(mGraphicsViewLeftImage);
    const QSignalBlocker blocker2(mGraphicsViewRightImage);

    if (QTreeWidgetItem *item = mTreeWidgetMatches->currentItem()) {

        QPointF query_point(item->text(2).toDouble(), item->text(3).toDouble());
        QPointF train_point(item->text(5).toDouble(), item->text(6).toDouble());

        mGraphicsViewLeftImage->scene()->clearSelection();
        mGraphicsViewRightImage->scene()->clearSelection();

        mGraphicsViewLeftImage->zoom11();
        mGraphicsViewLeftImage->centerOn(query_point);
        QPoint pt_left = mGraphicsViewLeftImage->mapFromScene(query_point);
        QGraphicsItem *select_item_left = mGraphicsViewLeftImage->itemAt(pt_left);
        if (select_item_left) {
            select_item_left->setSelected(true);
        }
        mGraphicsViewRightImage->zoom11();
        mGraphicsViewRightImage->centerOn(train_point);
        QPoint pt_right = mGraphicsViewRightImage->mapFromScene(train_point);
        QGraphicsItem *select_item_right = mGraphicsViewRightImage->itemAt(pt_right);
        if (select_item_right) {
            select_item_right->setSelected(true);
        }
    } else {
        mGraphicsViewLeftImage->scene()->clearSelection();
        mGraphicsViewRightImage->scene()->clearSelection();
    }
    update();
}

void MatchViewerViewImp::onGraphicsViewLeftSelectionChanged()
{
    const QSignalBlocker blocker1(mGraphicsViewRightImage);
    const QSignalBlocker blocker2(mTreeWidgetMatches);

    QList<QGraphicsItem *> items = mGraphicsViewLeftImage->items();
    bool bSelectedItem = false;
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->isSelected() == true) {

            mGraphicsViewRightImage->scene()->clearSelection();
            mTreeWidgetMatches->selectionModel()->clearSelection();

            QList<QGraphicsItem *> items_right = mGraphicsViewRightImage->items();
            items_right[i]->setSelected(true);

            for (int j = 0; j < mTreeWidgetMatches->topLevelItemCount(); j++) {
                QTreeWidgetItem *item = mTreeWidgetMatches->topLevelItem(j);
                if (item && item->text(0).compare(items[i]->toolTip()) == 0) {
                    item->setSelected(true);
                    QPointF train_point(item->text(5).toDouble(), item->text(6).toDouble());
                    mGraphicsViewRightImage->centerOn(train_point);

                    QModelIndex index = mTreeWidgetMatches->model()->index(j, 0);
                    mTreeWidgetMatches->scrollTo(index);

                    break;
                }
            }

            bSelectedItem = true;

            break;
        }
    }

    if (bSelectedItem == false) {
        for (auto &item : mGraphicsViewRightImage->items())
        {
            item->setSelected(false);
        }
        mTreeWidgetMatches->selectionModel()->clearSelection();
    }

    update();
}

void MatchViewerViewImp::onGraphicsViewRightSelectionChanged()
{
    const QSignalBlocker blocker1(mGraphicsViewLeftImage);
    const QSignalBlocker blocker2(mTreeWidgetMatches);

    QList<QGraphicsItem *> items = mGraphicsViewRightImage->items();
    bool bSelectedItem = false;
    for (int i = 0; i < items.size(); i++) {
        if (items[i]->isSelected() == true) {
            mGraphicsViewLeftImage->scene()->clearSelection();
            QList<QGraphicsItem *> items_left = mGraphicsViewLeftImage->items();
            items_left[i]->setSelected(true);
            mTreeWidgetMatches->selectionModel()->clearSelection();
            for (int j = 0; j < mTreeWidgetMatches->topLevelItemCount(); j++) {
                QTreeWidgetItem *itemProject = mTreeWidgetMatches->topLevelItem(j);
                if (itemProject && itemProject->text(0).compare(items[i]->toolTip()) == 0) {
                    itemProject->setSelected(true);
                    QPointF query_point(itemProject->text(2).toDouble(), itemProject->text(3).toDouble());
                    mGraphicsViewLeftImage->centerOn(query_point);

                    QModelIndex index = mTreeWidgetMatches->model()->index(j, 0);
                    mTreeWidgetMatches->scrollTo(index);

                    break;
                }
            }

            bSelectedItem = true;

            break;
        }
    }

    if (bSelectedItem == false) {
        for (auto &item : mGraphicsViewLeftImage->items())
        {
            item->setSelected(false);
        }
        mTreeWidgetMatches->selectionModel()->clearSelection();
    }

    update();
}

void MatchViewerViewImp::closeEvent(QCloseEvent *event)
{
    for (auto &item : mGraphicsViewLeftImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }

    for (auto &item : mGraphicsViewRightImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete key_point;
                key_point = nullptr;
            }
        }
    }

    QDialog::closeEvent(event);
}

} // namespace graphos
