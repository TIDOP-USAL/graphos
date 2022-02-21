/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

  mRightImage = new QLabel(this);

  gridLayout->addWidget(mRightImage, 0, 1, 1, 1);

  mComboBoxLeftImage = new QComboBox(this);
  gridLayout->addWidget(mComboBoxLeftImage, 1, 0, 1, 1);

  mComboBoxRightImage = new QComboBox(this);
  gridLayout->addWidget(mComboBoxRightImage, 1, 1, 1, 1);

  mGraphicsViewImage = new GraphicViewerImp(this);
  gridLayout->addWidget(mGraphicsViewImage, 2, 0, 1, 1);

  mGraphicsViewPseudoimage = new GraphicViewerImp(this);
  gridLayout->addWidget(mGraphicsViewPseudoimage, 2, 1, 1, 1);

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
  mButtonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 5, 0, 1, 2);

  QIcon iconZoomIn;
  iconZoomIn.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
  QIcon iconZoomOut;
  iconZoomOut.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
  QIcon iconZoomExtend;
  iconZoomExtend.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-magnifying-glass-with-expand-sign.png"), QSize(), QIcon::Normal, QIcon::Off);
  QIcon iconZoom11;
  iconZoom11.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-one-to-one.png"), QSize(), QIcon::Normal, QIcon::Off);

  QMenu *contextMenuLeft = new QMenu(mGraphicsViewImage);

  mActionLeftZoomIn = new QAction(this);
  mActionLeftZoomIn->setIcon(iconZoomIn);
  contextMenuLeft->addAction(mActionLeftZoomIn);

  mActionLeftZoomOut = new QAction(this);
  mActionLeftZoomOut->setIcon(iconZoomOut);
  contextMenuLeft->addAction(mActionLeftZoomOut);

  mActionLeftZoomExtend = new QAction(this);
  mActionLeftZoomExtend->setIcon(iconZoomExtend);
  contextMenuLeft->addAction(mActionLeftZoomExtend);

  mActionLeftZoom11 = new QAction(this);
  mActionLeftZoom11->setIcon(iconZoom11);
  contextMenuLeft->addAction(mActionLeftZoom11);

  mGraphicsViewImage->setContextMenu(contextMenuLeft);

  QMenu *contextMenuRight = new QMenu(mGraphicsViewPseudoimage);

  mActionRightZoomIn = new QAction(this);
  mActionRightZoomIn->setIcon(iconZoomIn);
  contextMenuRight->addAction(mActionRightZoomIn);

  mActionRightZoomOut = new QAction(this);
  mActionRightZoomOut->setIcon(iconZoomOut);
  contextMenuRight->addAction(mActionRightZoomOut);

  mActionRightZoomExtend = new QAction(this);
  mActionRightZoomExtend->setIcon(iconZoomExtend);
  contextMenuRight->addAction(mActionRightZoomExtend);

  mActionRightZoom11 = new QAction(this);
  mActionRightZoom11->setIcon(iconZoom11);
  contextMenuRight->addAction(mActionRightZoom11);

  mGraphicsViewPseudoimage->setContextMenu(contextMenuRight);

  this->retranslate();
  this->update();
}

void MatchViewerViewImp::initSignalAndSlots()
{
  connect(mComboBoxLeftImage,     SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxLeftImageIndexChanged(int)));
  connect(mComboBoxRightImage,    SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxRightImageIndexChanged(int)));
  connect(mTreeWidgetMatches,     SIGNAL(itemSelectionChanged()),   this, SLOT(onTreeWidgetMatchesItemSelectionChanged()));
  connect(mGraphicsViewImage,      SIGNAL(selectionChanged()),       this, SLOT(onGraphicsViewLeftSelectionChanged()));
  connect(mGraphicsViewPseudoimage,     SIGNAL(selectionChanged()),       this, SLOT(onGraphicsViewRightSelectionChanged()));
  connect(mActionLeftZoomIn,      SIGNAL(triggered(bool)), mGraphicsViewImage, SLOT(zoomIn()));
  connect(mActionLeftZoomOut,     SIGNAL(triggered(bool)), mGraphicsViewImage, SLOT(zoomOut()));
  connect(mActionLeftZoomExtend,  SIGNAL(triggered(bool)), mGraphicsViewImage, SLOT(zoomExtend()));
  connect(mActionLeftZoom11,      SIGNAL(triggered(bool)), mGraphicsViewImage, SLOT(zoom11()));
  connect(mActionRightZoomIn,     SIGNAL(triggered(bool)), mGraphicsViewPseudoimage, SLOT(zoomIn()));
  connect(mActionRightZoomOut,    SIGNAL(triggered(bool)), mGraphicsViewPseudoimage, SLOT(zoomOut()));
  connect(mActionRightZoomExtend, SIGNAL(triggered(bool)), mGraphicsViewPseudoimage, SLOT(zoomExtend()));
  connect(mActionRightZoom11,     SIGNAL(triggered(bool)), mGraphicsViewPseudoimage, SLOT(zoom11()));

  connect(mButtonBox->button(QDialogButtonBox::Close),  SIGNAL(clicked(bool)), this, SLOT(accept()));
  connect(mButtonBox->button(QDialogButtonBox::Help),   SIGNAL(clicked(bool)), this, SIGNAL(help()));
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
  mRightImage->setText(QApplication::translate("MatchViewerView", "Right Image:"));
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
  mActionLeftZoomIn->setText(QApplication::translate("MatchViewerView", "Zoom In"));
  mActionLeftZoomOut->setText(QApplication::translate("MatchViewerView", "Zoom Out"));
  mActionLeftZoomExtend->setText(QApplication::translate("MatchViewerView", "Zoom Extend"));
  mActionLeftZoom11->setText(QApplication::translate("MatchViewerView", "Zoom 1:1"));
  mActionRightZoomIn->setText(QApplication::translate("MatchViewerView", "Zoom In"));
  mActionRightZoomOut->setText(QApplication::translate("MatchViewerView", "Zoom Out"));
  mActionRightZoomExtend->setText(QApplication::translate("MatchViewerView", "Zoom Extend"));
  mActionRightZoom11->setText(QApplication::translate("MatchViewerView", "Zoom 1:1"));
  mButtonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
  mButtonBox->button(QDialogButtonBox::Help)->setText("Help");
}

void MatchViewerViewImp::setLeftImage(const QString &leftImage)
{
  QSignalBlocker blocker(mComboBoxLeftImage);
  mComboBoxLeftImage->setCurrentText(leftImage);
  mGraphicsViewImage->scene()->clearSelection();
  QString image = mComboBoxLeftImage->currentData().toString();
  std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
  imageReader->open();
  if (imageReader->isOpen()) {
    cv::Mat bmp = imageReader->read();
    mGraphicsViewImage->setImage(cvMatToQImage(bmp));
    imageReader->close();
  }
}

void MatchViewerViewImp::setRightImage(const QString &rightImage)
{
  QSignalBlocker blocker(mComboBoxRightImage);
  mComboBoxRightImage->setCurrentText(rightImage);
  mGraphicsViewPseudoimage->scene()->clearSelection();
  QString image = mComboBoxRightImage->currentData().toString();
  std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
  imageReader->open();
  if (imageReader->isOpen()) {
    cv::Mat bmp = imageReader->read();
    mGraphicsViewPseudoimage->setImage(cvMatToQImage(bmp));
    imageReader->close();
  }
}

void MatchViewerViewImp::setLeftImageList(const std::vector<QString> &leftImageList)
{
  QSignalBlocker blocker(mComboBoxLeftImage);
  mComboBoxLeftImage->clear();
  for (auto &image : leftImageList){
    QFileInfo file_info(image);
    mComboBoxLeftImage->addItem(file_info.baseName(), image);
  }
}

void MatchViewerViewImp::setRightImageList(const std::vector<QString> &rightImageList)
{
  QSignalBlocker blocker(mComboBoxRightImage);
  mComboBoxRightImage->clear();
  for (auto &image : rightImageList){
    QFileInfo file_info(image);
    mComboBoxRightImage->addItem(file_info.baseName(), image);
  }
}

void MatchViewerViewImp::setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF> > &matches)
{
  QSignalBlocker blocker(mTreeWidgetMatches);
  mTreeWidgetMatches->clear();

  for (auto &item : mGraphicsViewImage->scene()->items()) {
    if (mMarkerType == 0){
      // Circle
      if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 1){
      // Cross
      if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 2){
      // Diagonal cross
      if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    }
  }

  for (auto &item : mGraphicsViewPseudoimage->scene()->items()) {
    if (mMarkerType == 0){
      // Circle
      if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 1){
      // Cross
      if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 2){
      // Diagonal cross
      if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    }
  }


  mGraphicsViewImage->zoomExtend();
  mGraphicsViewPseudoimage->zoomExtend();

  QPen pen(QColor(mMarkerColor), mMarkerWidth);
  pen.setCosmetic(true);
  QPen select_pen(QColor(mSelectedMarkerColor), mSelectedMarkerWidth);
  select_pen.setCosmetic(true);

  for (size_t i = 0; i < matches.size(); i++){
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

    if (mMarkerType == 0){
      // Circle
      CircleGraphicItem *itemLeft = new CircleGraphicItem(query_point, mMarkerSize);
      itemLeft->setPen(pen);
      itemLeft->setSelectedPen(select_pen);
      itemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
      itemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewImage->scene()->addItem(itemLeft);
      CircleGraphicItem *itemRight = new CircleGraphicItem(train_point, mMarkerSize);
      itemRight->setPen(pen);
      itemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
      itemRight->setSelectedPen(select_pen);
      itemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewPseudoimage->scene()->addItem(itemRight);
    } else if (mMarkerType == 1){
      // Cross
      CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(query_point, mMarkerSize);
      crossGraphicItemLeft->setPen(pen);
      crossGraphicItemLeft->setSelectedPen(select_pen);
      crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
      crossGraphicItemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewImage->scene()->addItem(crossGraphicItemLeft);
      CrossGraphicItem *crossGraphicItemRight = new CrossGraphicItem(train_point, mMarkerSize);
      crossGraphicItemRight->setPen(pen);
      crossGraphicItemRight->setSelectedPen(select_pen);
      crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
      crossGraphicItemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewPseudoimage->scene()->addItem(crossGraphicItemRight);
    } else if (mMarkerType == 2){
      // Diagonal cross
      DiagonalCrossGraphicItem *crossGraphicItemLeft = new DiagonalCrossGraphicItem(query_point, mMarkerSize);
      crossGraphicItemLeft->setPen(pen);
      crossGraphicItemLeft->setSelectedPen(select_pen);
      crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
      crossGraphicItemLeft->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewImage->scene()->addItem(crossGraphicItemLeft);
      DiagonalCrossGraphicItem *crossGraphicItemRight = new DiagonalCrossGraphicItem(train_point, mMarkerSize);
      crossGraphicItemRight->setPen(pen);
      crossGraphicItemRight->setSelectedPen(select_pen);
      crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
      crossGraphicItemRight->setToolTip(QString::number(static_cast<int>(pp_id)));
      mGraphicsViewPseudoimage->scene()->addItem(crossGraphicItemRight);
    }
  }
}

void MatchViewerViewImp::setBGColor(const QString &bgColor)
{
  mGraphicsViewImage->setBackgroundBrush(QBrush(QColor(bgColor)));
  mGraphicsViewPseudoimage->setBackgroundBrush(QBrush(QColor(bgColor)));
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

  if (markerTypeOld != mMarkerType){

    for (auto &item : mGraphicsViewImage->scene()->items()) {

      QPointF point;
      int id = 0;

      if (markerTypeOld == 0) {
        // Circle
        if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
          mGraphicsViewImage->scene()->removeItem(item);
          point = item->pos();
          id = item->toolTip().toInt();
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else if (markerTypeOld == 1) {
        // Cross
        if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
          mGraphicsViewImage->scene()->removeItem(item);
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else if (markerTypeOld == 2) {
        // Diagonal cross
        if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
          mGraphicsViewImage->scene()->removeItem(item);
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else {
        continue;
      }

      if (mMarkerType == 0){
        // Circle
        CircleGraphicItem *itemLeft = new CircleGraphicItem(point, mMarkerSize);
        itemLeft->setPen(pen);
        itemLeft->setSelectedPen(select_pen);
        itemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
        itemLeft->setToolTip(QString::number(id));
        mGraphicsViewImage->scene()->addItem(itemLeft);
      } else if (mMarkerType == 1) {
        // Cross
        CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(point, mMarkerSize);
        crossGraphicItemLeft->setPen(pen);
        crossGraphicItemLeft->setSelectedPen(select_pen);
        crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
        crossGraphicItemLeft->setToolTip(QString::number(id));
        mGraphicsViewImage->scene()->addItem(crossGraphicItemLeft);
      } else if (mMarkerType == 2) {
        // Diagonal cross
        DiagonalCrossGraphicItem *crossGraphicItemLeft = new DiagonalCrossGraphicItem(point, mMarkerSize);
        crossGraphicItemLeft->setPen(pen);
        crossGraphicItemLeft->setSelectedPen(select_pen);
        crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
        crossGraphicItemLeft->setToolTip(QString::number(id));
        mGraphicsViewImage->scene()->addItem(crossGraphicItemLeft);
      }
    }

    for (auto &item : mGraphicsViewPseudoimage->scene()->items()) {

      QPointF point;
      int id = 0;

      if (markerTypeOld == 0){
        // Circle
        if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
          mGraphicsViewPseudoimage->scene()->removeItem(item);
          point = item->pos();
          id = item->toolTip().toInt();
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else if (markerTypeOld == 1){
        // Cross
        if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
          mGraphicsViewPseudoimage->scene()->removeItem(item);
          point = item->pos();
          id = item->toolTip().toInt();
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else if (markerTypeOld == 2){
        // Diagonal cross
        if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
          mGraphicsViewPseudoimage->scene()->removeItem(item);
          point = item->pos();
          id = item->toolTip().toInt();
          delete keyPoint;
          keyPoint = nullptr;
        }
      } else {
        continue;
      }

      if (mMarkerType == 0){
        // Circle
        CircleGraphicItem *itemRight = new CircleGraphicItem(point, mMarkerSize);
        itemRight->setPen(pen);
        itemRight->setSelectedPen(select_pen);
        itemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
        itemRight->setToolTip(QString::number(id));
        mGraphicsViewPseudoimage->scene()->addItem(itemRight);
      } else if (mMarkerType == 1){
        // Cross
        CrossGraphicItem *crossGraphicItemRight = new CrossGraphicItem(point, mMarkerSize);
        crossGraphicItemRight->setPen(pen);
        crossGraphicItemRight->setSelectedPen(select_pen);
        crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
        crossGraphicItemRight->setToolTip(QString::number(id));
        mGraphicsViewPseudoimage->scene()->addItem(crossGraphicItemRight);
      } else if (mMarkerType == 2){
        // Diagonal cross
        DiagonalCrossGraphicItem *crossGraphicItemRight = new DiagonalCrossGraphicItem(point, mMarkerSize);
        crossGraphicItemRight->setPen(pen);
        crossGraphicItemRight->setSelectedPen(select_pen);
        crossGraphicItemRight->setFlag(QGraphicsItem::ItemIsSelectable, true);
        crossGraphicItemRight->setToolTip(QString::number(id));
        mGraphicsViewPseudoimage->scene()->addItem(crossGraphicItemRight);
      }
    }

  } else {

    for (auto &item : mGraphicsViewImage->scene()->items()) {
      if (mMarkerType == 0){
        // Circle
        if (CircleGraphicItem *keyPoints = dynamic_cast<CircleGraphicItem *>(item)){
          keyPoints->setPen(pen);
          keyPoints->setSelectedPen(select_pen);
          keyPoints->setSize(mMarkerSize);
        }
      } else if (mMarkerType == 1){
        // Cross
        if (CrossGraphicItem *keyPoints = dynamic_cast<CrossGraphicItem *>(item)){
          keyPoints->setPen(pen);
          keyPoints->setSelectedPen(select_pen);
          keyPoints->setSize(mMarkerSize);
        }
      } else if (mMarkerType == 2){
        // Diagonal cross
        if (DiagonalCrossGraphicItem *keyPoints = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
          keyPoints->setPen(pen);
          keyPoints->setSelectedPen(select_pen);
          keyPoints->setSize(mMarkerSize);
        }
      }
    }

    for (auto &item : mGraphicsViewPseudoimage->scene()->items()) {
      if (mMarkerType == 0) {
        // Circle
        if (CircleGraphicItem *keyPoints = dynamic_cast<CircleGraphicItem *>(item)){
          keyPoints->setPen(pen);
          keyPoints->setSelectedPen(select_pen);
          keyPoints->setSize(mMarkerSize);
        }
      } else if (mMarkerType == 1) {
        // Cross
        if (CrossGraphicItem *keyPoints = dynamic_cast<CrossGraphicItem *>(item)){
          keyPoints->setPen(pen);
          keyPoints->setSelectedPen(select_pen);
          keyPoints->setSize(mMarkerSize);
        }
      } else if (mMarkerType == 2) {
        // Diagonal cross
        if (DiagonalCrossGraphicItem *keyPoints = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
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



void MatchViewerViewImp::onComboBoxLeftImageIndexChanged(int idx)
{
  emit leftImageChange(mComboBoxLeftImage->itemText(idx));
}

void MatchViewerViewImp::onComboBoxRightImageIndexChanged(int idx)
{
  QString image_right(mComboBoxRightImage->itemText(idx));
  QString image_left(mComboBoxLeftImage->currentText());
  emit rightImageChange(image_right);
  emit loadMatches(image_left, image_right);
}

void MatchViewerViewImp::onTreeWidgetMatchesItemClicked(QTreeWidgetItem *item, int col)
{
  QPointF query_point(item->text(1).toDouble(), item->text(2).toDouble());
  QPointF train_point(item->text(3).toDouble(), item->text(4).toDouble());

  mGraphicsViewImage->zoom11();
  mGraphicsViewImage->centerOn(query_point);
  mGraphicsViewPseudoimage->zoom11();
  mGraphicsViewPseudoimage->centerOn(train_point);
}

void MatchViewerViewImp::onTreeWidgetMatchesItemSelectionChanged()
{
  const QSignalBlocker blocker1(mGraphicsViewImage);
  const QSignalBlocker blocker2(mGraphicsViewPseudoimage);

  if (QTreeWidgetItem *item = mTreeWidgetMatches->currentItem()){

    QPointF query_point(item->text(2).toDouble(), item->text(3).toDouble());
    QPointF train_point(item->text(5).toDouble(), item->text(6).toDouble());

    mGraphicsViewImage->scene()->clearSelection();
    mGraphicsViewPseudoimage->scene()->clearSelection();

    mGraphicsViewImage->zoom11();
    mGraphicsViewImage->centerOn(query_point);
    QPoint pt_left = mGraphicsViewImage->mapFromScene(query_point);
    QGraphicsItem *select_item_left = mGraphicsViewImage->itemAt(pt_left);
    if (select_item_left) {
      select_item_left->setSelected(true);
    }
    mGraphicsViewPseudoimage->zoom11();
    mGraphicsViewPseudoimage->centerOn(train_point);
    QPoint pt_right = mGraphicsViewPseudoimage->mapFromScene(train_point);
    QGraphicsItem *select_item_right = mGraphicsViewPseudoimage->itemAt(pt_right);
    if (select_item_right) {
      select_item_right->setSelected(true);
    }
  } else {
    mGraphicsViewImage->scene()->clearSelection();
    mGraphicsViewPseudoimage->scene()->clearSelection();
  }
  update();
}

void MatchViewerViewImp::onGraphicsViewLeftSelectionChanged()
{
  const QSignalBlocker blocker1(mGraphicsViewPseudoimage);
  const QSignalBlocker blocker2(mTreeWidgetMatches);

  QList<QGraphicsItem *> items = mGraphicsViewImage->items();
  bool bSelectedItem = false;
  for (int i = 0; i < items.size(); i++){
    if (items[i]->isSelected() == true) {
      mGraphicsViewPseudoimage->scene()->clearSelection();
      QList<QGraphicsItem *> items_right = mGraphicsViewPseudoimage->items();
      items_right[i]->setSelected(true);
      mTreeWidgetMatches->selectionModel()->clearSelection();
      for (int j = 0; j < mTreeWidgetMatches->topLevelItemCount(); j++){
        QTreeWidgetItem *item = mTreeWidgetMatches->topLevelItem(j);
        if (item && item->text(0).compare(items[i]->toolTip()) == 0){
          item->setSelected(true);
          QPointF train_point(item->text(5).toDouble(), item->text(6).toDouble());
          mGraphicsViewPseudoimage->centerOn(train_point);

          QModelIndex index = mTreeWidgetMatches->model()->index(j, 0);
          mTreeWidgetMatches->scrollTo(index);

          break;
        }
      }

      bSelectedItem = true;

      break;
    }
  }

  if (bSelectedItem == false){
    QList<QGraphicsItem *> items = mGraphicsViewPseudoimage->items();
    for (int i = 0; i < items.size(); i++){
      items[i]->setSelected(false);
    }
    mTreeWidgetMatches->selectionModel()->clearSelection();
  }

  update();
}

void MatchViewerViewImp::onGraphicsViewRightSelectionChanged()
{
  const QSignalBlocker blocker1(mGraphicsViewImage);
  const QSignalBlocker blocker2(mTreeWidgetMatches);

  QList<QGraphicsItem *> items = mGraphicsViewPseudoimage->items();
  bool bSelectedItem = false;
  for (int i = 0; i < items.size(); i++){
    if (items[i]->isSelected() == true) {
      mGraphicsViewImage->scene()->clearSelection();
      QList<QGraphicsItem *> items_left = mGraphicsViewImage->items();
      items_left[i]->setSelected(true);
      mTreeWidgetMatches->selectionModel()->clearSelection();
      for (int j = 0; j < mTreeWidgetMatches->topLevelItemCount(); j++){
        QTreeWidgetItem *itemProject = mTreeWidgetMatches->topLevelItem(j);
        if (itemProject && itemProject->text(0).compare(items[i]->toolTip()) == 0){
          itemProject->setSelected(true);
          QPointF query_point(itemProject->text(2).toDouble(), itemProject->text(3).toDouble());
          mGraphicsViewImage->centerOn(query_point);

          QModelIndex index = mTreeWidgetMatches->model()->index(j, 0);
          mTreeWidgetMatches->scrollTo(index);

          break;
        }
      }

      bSelectedItem = true;

      break;
    }
  }

  if (bSelectedItem == false){
    QList<QGraphicsItem *> items = mGraphicsViewImage->items();
    for (int i = 0; i < items.size(); i++){
      items[i]->setSelected(false);
    }
    mTreeWidgetMatches->selectionModel()->clearSelection();
  }

  update();
}

void MatchViewerViewImp::closeEvent(QCloseEvent *event)
{
  for (auto &item : mGraphicsViewImage->scene()->items()) {
    if (mMarkerType == 0){
      // Circle
      if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 1){
      // Cross
      if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 2){
      // Diagonal cross
      if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
        mGraphicsViewImage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    }
  }

  for (auto &item : mGraphicsViewPseudoimage->scene()->items()) {
    if (mMarkerType == 0){
      // Circle
      if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 1){
      // Cross
      if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    } else if (mMarkerType == 2){
      // Diagonal cross
      if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)){
        mGraphicsViewPseudoimage->scene()->removeItem(item);
        delete keyPoint;
        keyPoint = nullptr;
      }
    }
  }
}

} // namespace graphos
