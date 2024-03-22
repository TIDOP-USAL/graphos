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
    this->initUI();
    this->initSignalAndSlots();
}

MatchViewerViewImp::~MatchViewerViewImp()
{
}

void MatchViewerViewImp::initUI()
{
    this->setObjectName(QString("MatchViewerView"));

    this->resize(994, 688);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabelLeftImage = new QLabel(this);
    gridLayout->addWidget(mLabelLeftImage, 0, 0, 1, 1);

    mLabelRightImage = new QLabel(this);
    gridLayout->addWidget(mLabelRightImage, 0, 1, 1, 1);

    mComboBoxLeftImage = new QComboBox(this);
    gridLayout->addWidget(mComboBoxLeftImage, 1, 0, 1, 1);

    mComboBoxRightImage = new QComboBox(this);
    gridLayout->addWidget(mComboBoxRightImage, 1, 1, 1, 1);

    mGraphicsViewLeftImage = new GraphicViewer(this);
    gridLayout->addWidget(mGraphicsViewLeftImage, 2, 0, 1, 1);

    mGraphicsViewRightImage = new GraphicViewer(this);
    gridLayout->addWidget(mGraphicsViewRightImage, 2, 1, 1, 1);

    mLabelMatches = new QLabel(this);
    gridLayout->addWidget(mLabelMatches, 3, 0, 1, 1);

    QGridLayout *gridLayout2 = new QGridLayout();

    mTreeWidgetMatches = new QTreeWidget(this);
    mTreeWidgetMatches->setMaximumSize(QSize(16777215, 285));
    mTreeWidgetMatches->setAlternatingRowColors(true);

    gridLayout2->addWidget(mTreeWidgetMatches, 0, 0, 4, 1);

    mPushButtonDeleteMatch = new QPushButton(this);
    mPushButtonDeleteMatch->setMaximumSize(QSize(280, 16777215));
    gridLayout2->addWidget(mPushButtonDeleteMatch, 0, 1, 1, 1);
    /// Desactivo el borrado de puntos por ahora
    mPushButtonDeleteMatch->setVisible(false);

    gridLayout->addLayout(gridLayout2, 4, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Close | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 5, 0, 1, 2);

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
    mPushButtonDeleteMatch->setEnabled(mTreeWidgetMatches->selectedItems().size() > 0);
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

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(imageLeft.toStdString());
    imageReader->open();
    if (imageReader->isOpen()) {
        cv::Mat bmp = imageReader->read();
        mGraphicsViewLeftImage->setImage(cvMatToQImage(bmp));
        imageReader->close();
    }
}

void MatchViewerViewImp::setRightImage(const QString &imageRight)
{
    QSignalBlocker blocker(mComboBoxRightImage);
    QFileInfo file_info(imageRight);
    mComboBoxRightImage->setCurrentText(imageRight);
    mGraphicsViewRightImage->scene()->clearSelection();

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(imageRight.toStdString());
    imageReader->open();
    if (imageReader->isOpen()) {
        cv::Mat bmp = imageReader->read();
        mGraphicsViewRightImage->setImage(cvMatToQImage(bmp));
        imageReader->close();
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
            if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        }
    }

    for (auto &item : mGraphicsViewRightImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        }
    }


    mGraphicsViewLeftImage->zoomExtend();
    mGraphicsViewRightImage->zoomExtend();

    QPen pen(QColor(mMarkerColor), mMarkerWidth);
    pen.setCosmetic(true);
    QPen select_pen(QColor(mSelectedMarkerColor), mSelectedMarkerWidth);
    select_pen.setCosmetic(true);

    for (size_t i = 0; i < matches.size(); i++) {
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        size_t pp_id, query_id, train_id;
        QPointF query_point, train_point;
        std::tie(pp_id, query_id, query_point, train_id, train_point) = matches[i];
        treeWidgetItem->setText(0, QString::number(pp_id));
        treeWidgetItem->setText(1, QString::number(query_id));
        treeWidgetItem->setText(2, QString::number(query_point.x()));
        treeWidgetItem->setText(3, QString::number(query_point.y()));
        treeWidgetItem->setText(4, QString::number(train_id));
        treeWidgetItem->setText(5, QString::number(train_point.x()));
        treeWidgetItem->setText(6, QString::number(train_point.y()));
        mTreeWidgetMatches->addTopLevelItem(treeWidgetItem);

        if (mMarkerType == 0) {
            // Circle
            CircleGraphicItem *itemLeft = new CircleGraphicItem(query_point, mMarkerSize);
            itemLeft->setPen(pen);
            itemLeft->setSelectedPen(select_pen);
            itemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
            itemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(itemLeft);
            CircleGraphicItem *itemRight = new CircleGraphicItem(train_point, mMarkerSize);
            itemRight->setPen(pen);
            itemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
            itemRight->setSelectedPen(select_pen);
            itemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(itemRight);
        } else if (mMarkerType == 1) {
            // Cross
            CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(query_point, mMarkerSize);
            crossGraphicItemLeft->setPen(pen);
            crossGraphicItemLeft->setSelectedPen(select_pen);
            crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
            crossGraphicItemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(crossGraphicItemLeft);
            CrossGraphicItem *crossGraphicItemRight = new CrossGraphicItem(train_point, mMarkerSize);
            crossGraphicItemRight->setPen(pen);
            crossGraphicItemRight->setSelectedPen(select_pen);
            crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
            crossGraphicItemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(crossGraphicItemRight);
        } else if (mMarkerType == 2) {
            // Diagonal cross
            DiagonalCrossGraphicItem *crossGraphicItemLeft = new DiagonalCrossGraphicItem(query_point, mMarkerSize);
            crossGraphicItemLeft->setPen(pen);
            crossGraphicItemLeft->setSelectedPen(select_pen);
            crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
            crossGraphicItemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewLeftImage->scene()->addItem(crossGraphicItemLeft);
            DiagonalCrossGraphicItem *crossGraphicItemRight = new DiagonalCrossGraphicItem(train_point, mMarkerSize);
            crossGraphicItemRight->setPen(pen);
            crossGraphicItemRight->setSelectedPen(select_pen);
            crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
            crossGraphicItemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
            mGraphicsViewRightImage->scene()->addItem(crossGraphicItemRight);
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
                if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else if (markerTypeOld == 1) {
                // Cross
                if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else if (markerTypeOld == 2) {
                // Diagonal cross
                if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    mGraphicsViewLeftImage->scene()->removeItem(item);
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else {
                continue;
            }

            if (mMarkerType == 0) {
                // Circle
                CircleGraphicItem *itemLeft = new CircleGraphicItem(point, mMarkerSize);
                itemLeft->setPen(pen);
                itemLeft->setSelectedPen(select_pen);
                itemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
                itemLeft->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(itemLeft);
            } else if (mMarkerType == 1) {
                // Cross
                CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(point, mMarkerSize);
                crossGraphicItemLeft->setPen(pen);
                crossGraphicItemLeft->setSelectedPen(select_pen);
                crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
                crossGraphicItemLeft->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(crossGraphicItemLeft);
            } else if (mMarkerType == 2) {
                // Diagonal cross
                DiagonalCrossGraphicItem *crossGraphicItemLeft = new DiagonalCrossGraphicItem(point, mMarkerSize);
                crossGraphicItemLeft->setPen(pen);
                crossGraphicItemLeft->setSelectedPen(select_pen);
                crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
                crossGraphicItemLeft->setToolTip(QString::number(id));
                mGraphicsViewLeftImage->scene()->addItem(crossGraphicItemLeft);
            }
        }

        for (auto &item : mGraphicsViewRightImage->scene()->items()) {

            QPointF point;
            int id = 0;

            if (markerTypeOld == 0) {
                // Circle
                if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else if (markerTypeOld == 1) {
                // Cross
                if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else if (markerTypeOld == 2) {
                // Diagonal cross
                if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    mGraphicsViewRightImage->scene()->removeItem(item);
                    point = item->pos();
                    id = item->toolTip().toInt();
                    delete keyPoint;
                    keyPoint = nullptr;
                }
            } else {
                continue;
            }

            if (mMarkerType == 0) {
                // Circle
                CircleGraphicItem *itemRight = new CircleGraphicItem(point, mMarkerSize);
                itemRight->setPen(pen);
                itemRight->setSelectedPen(select_pen);
                itemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
                itemRight->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(itemRight);
            } else if (mMarkerType == 1) {
                // Cross
                CrossGraphicItem *crossGraphicItemRight = new CrossGraphicItem(point, mMarkerSize);
                crossGraphicItemRight->setPen(pen);
                crossGraphicItemRight->setSelectedPen(select_pen);
                crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
                crossGraphicItemRight->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(crossGraphicItemRight);
            } else if (mMarkerType == 2) {
                // Diagonal cross
                DiagonalCrossGraphicItem *crossGraphicItemRight = new DiagonalCrossGraphicItem(point, mMarkerSize);
                crossGraphicItemRight->setPen(pen);
                crossGraphicItemRight->setSelectedPen(select_pen);
                crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
                crossGraphicItemRight->setToolTip(QString::number(id));
                mGraphicsViewRightImage->scene()->addItem(crossGraphicItemRight);
            }
        }

    } else {

        for (auto &item : mGraphicsViewLeftImage->scene()->items()) {
            if (mMarkerType == 0) {
                // Circle
                if (CircleGraphicItem *keyPoints = dynamic_cast<CircleGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 1) {
                // Cross
                if (CrossGraphicItem *keyPoints = dynamic_cast<CrossGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 2) {
                // Diagonal cross
                if (DiagonalCrossGraphicItem *keyPoints = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
                }
            }
        }

        for (auto &item : mGraphicsViewRightImage->scene()->items()) {
            if (mMarkerType == 0) {
                // Circle
                if (CircleGraphicItem *keyPoints = dynamic_cast<CircleGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 1) {
                // Cross
                if (CrossGraphicItem *keyPoints = dynamic_cast<CrossGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
                }
            } else if (mMarkerType == 2) {
                // Diagonal cross
                if (DiagonalCrossGraphicItem *keyPoints = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                    keyPoints->setPen(pen);
                    keyPoints->setSelectedPen(select_pen);
                    keyPoints->setSize(mMarkerSize);
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

void MatchViewerViewImp::onTreeWidgetMatchesItemClicked(QTreeWidgetItem *item, int col)
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
        QList<QGraphicsItem *> items = mGraphicsViewRightImage->items();
        for (int i = 0; i < items.size(); i++) {
            items[i]->setSelected(false);
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
        QList<QGraphicsItem *> items = mGraphicsViewLeftImage->items();
        for (int i = 0; i < items.size(); i++) {
            items[i]->setSelected(false);
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
            if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewLeftImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        }
    }

    for (auto &item : mGraphicsViewRightImage->scene()->items()) {
        if (mMarkerType == 0) {
            // Circle
            if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 1) {
            // Cross
            if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        } else if (mMarkerType == 2) {
            // Diagonal cross
            if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
                mGraphicsViewRightImage->scene()->removeItem(item);
                delete keyPoint;
                keyPoint = nullptr;
            }
        }
    }

    QDialog::closeEvent(event);
}

} // namespace graphos
