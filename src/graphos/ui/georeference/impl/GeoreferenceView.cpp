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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "GeoreferenceView.h"

#include "graphos/ui/utils/GraphicViewer.h"
#include "graphos/ui/utils/GraphicItem.h"
#include "graphos/core/utils.h"

#include <tidop/img/imgreader.h>

//#include <QFileDialog>
//#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
//#include <QTreeWidget>
#include <QTableView>
#include <QFileDialog>
#include <QComboBox>
#include <QSpacerItem>
#include <QStandardItemModel>


namespace graphos
{

GeoreferenceViewImp::GeoreferenceViewImp(QWidget *parent,
                                         Qt::WindowFlags f)
  : GeoreferenceView(parent, f),
    mProjectPath(""),
    bSelectedItem(false)
{
  this->initUI();
  this->initSignalAndSlots();
}

GeoreferenceViewImp::~GeoreferenceViewImp()
{

}

void GeoreferenceViewImp::onGraphicsViewSelectionChanged()
{
  const QSignalBlocker blocker1(mGraphicViewerWidget);
  //const QSignalBlocker blocker2(mTreeWidget);

  QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
  bSelectedItem = false;
  for (int i = 0; i < items.size(); i++) {
    if (items[i] && items[i]->isSelected() == true) {

  //    mTreeWidget->selectionModel()->clearSelection();
  //    for (int j = 0; j < mTreeWidget->topLevelItemCount(); j++){
  //      QTreeWidgetItem *item = mTreeWidget->topLevelItem(j);
  //      if (item && item->text(0).compare(items[i]->toolTip()) == 0){
  //        item->setSelected(true);
  //        QModelIndex index = mTreeWidget->model()->index(j, 0);
  //        mTreeWidget->scrollTo(index);
  //        break;
  //      }
  //    }

      bSelectedItem = true;
      break;
    }
  }

  update();
}

void GeoreferenceViewImp::onGraphicsViewRemoveSelectItems()
{
  //QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
  //QModelIndexList modelIndexList = selecctionModel->selectedRows();
  //while(modelIndexList.size() > 0){
  //  int index = modelIndexList.at(0).row();
  //  emit removeGroundControlPoint(index);
  //  modelIndexList = selecctionModel->selectedRows();
  //}
  const QSignalBlocker blocker1(mGraphicViewerWidget);
  //const QSignalBlocker blocker2(mComboBoxImages);
  QString image = mComboBoxImages->currentText();


  //QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
  //QModelIndexList modelIndexList = selecctionModel->selectedRows();
  //while(modelIndexList.size() > 0){
  //  int index = modelIndexList.at(0).row();
  //  emit removeGroundControlPoint(index);
  //  modelIndexList = selecctionModel->selectedRows();
  //}

  QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
  for (int i = 0; i < items.size(); i++) {
    if (items[i] && items[i]->isSelected() == true) {
      QString gcp = items[i]->toolTip();
      mGraphicViewerWidget->scene()->removeItem(items[i]);
      emit removeImagePoint(gcp, image);
  //    mTreeWidget->selectionModel()->clearSelection();
  //    for (int j = 0; j < mTreeWidget->topLevelItemCount(); j++){
  //      QTreeWidgetItem *item = mTreeWidget->topLevelItem(j);
  //      if (item && item->text(0).compare(items[i]->toolTip()) == 0){
  //        item->setSelected(true);
  //        QModelIndex index = mTreeWidget->model()->index(j, 0);
  //        mTreeWidget->scrollTo(index);
  //        break;
  //      }
  //    }

    }
  }
}

void GeoreferenceViewImp::selectionChanged(const QItemSelection &selected, 
                                           const QItemSelection &deselected)
{
  this->setEnableImagePointsAddOrEdit(false);
  QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();

  QModelIndexList modelIndexList = selecctionModel->selectedRows();
   
  /// Por ahora...
  this->setEnableImagePointsAddOrEdit(modelIndexList.size() == 1);

  for (auto modelIndex : modelIndexList) {
    // Se obtiene el identificador de la fila seleccionada y se filtran los puntos imagen.
    int index = modelIndex.row();
    //mTableViewGroundControlPoints->
  }

  this->update();
}

void GeoreferenceViewImp::clickedPoint(const QPointF &point)
{
  
  QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
  QModelIndexList modelIndexList = selecctionModel->selectedRows();

  int index = -1;
  if (modelIndexList.size() == 1) {
    index = modelIndexList.at(0).row();
    
  } else {
    /// Si no hay nada activado mostrar lista de Ground Control Points para seleccionar
  }

  if (index != -1) {
    QString groundControlPoint = mTableViewGroundControlPoints->model()->index(index, 0).data().toString();
    QString image = mComboBoxImages->currentText();
    emit addImagePoint(groundControlPoint, image, point);

    QPen pen(QColor("#FF0000"), 1);
    pen.setCosmetic(true);
    QPen select_pen(QColor("#FF0000"), 2);
    select_pen.setCosmetic(true);

    QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
    for (int i = 0; i < items.size(); i++) {
      if (items[i]->toolTip().compare(groundControlPoint) == 0) {
        mGraphicViewerWidget->scene()->removeItem(items[i]);
        break;
      }
    }

    CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(point, 20);
    crossGraphicItemLeft->setPen(pen);
    crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
    crossGraphicItemLeft->setToolTip(groundControlPoint);
    crossGraphicItemLeft->setSelectedPen(select_pen);
    mGraphicViewerWidget->scene()->addItem(crossGraphicItemLeft);
  }
}

//void GeoreferenceViewImp::onTreeWidgetItemSelectionChanged()
//{
//  const QSignalBlocker blocker1(mGraphicViewerWidget);
//
//  if (QTreeWidgetItem *item = mTreeWidgetGroundControlPoints->currentItem()){
//
//    QPointF point(item->text(1).toDouble(), item->text(2).toDouble());
//    mGraphicViewerWidget->scene()->clearSelection();
//
//    mGraphicViewerWidget->zoom11();
//    mGraphicViewerWidget->centerOn(point);
//    QPoint pt_scene = mGraphicViewerWidget->mapFromScene(point);
//    QGraphicsItem *select_item_left = mGraphicViewerWidget->itemAt(pt_scene);
//    if (select_item_left) {
//      select_item_left->setSelected(true);
//    }
//
//  } else {
//    mGraphicViewerWidget->scene()->clearSelection();
//  }
//  update();
//}

void GeoreferenceViewImp::setImageList(const std::vector<QString> &imageList)
{
  QSignalBlocker blocker(mComboBoxImages);
  mComboBoxImages->clear();
  for (auto &image : imageList){
    QFileInfo file_info(image);
    mComboBoxImages->addItem(file_info.baseName(), image);
  }
}

void GeoreferenceViewImp::setCurrentImage(const QString &leftImage)
{
  QSignalBlocker blocker(mComboBoxImages);
  mComboBoxImages->setCurrentText(leftImage);
  mGraphicViewerWidget->scene()->clearSelection();
  QString image = mComboBoxImages->currentData().toString();
  std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::createReader(image.toStdString());
  imageReader->open();
  if (imageReader->isOpen()) {
    cv::Mat bmp = imageReader->read();
    mGraphicViewerWidget->setImage(cvMatToQImage(bmp));
    imageReader->close();
  }
  mGraphicViewerWidget->zoomExtend();
}

void GeoreferenceViewImp::setItemModelGroundControlPoints(QAbstractItemModel *model)
{
  mTableViewGroundControlPoints->setModel(model);
  QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
  connect(selecctionModel, &QItemSelectionModel::selectionChanged, this, &GeoreferenceViewImp::selectionChanged);
}

void GeoreferenceViewImp::setItemModelImagePoints(QAbstractItemModel *model)
{
  mTableViewImagePoints->setModel(model);
}

void GeoreferenceViewImp::setEnableImagePointsAddOrEdit(bool active)
{
  if (active)
    connect(mGraphicViewerWidget, SIGNAL(mouseClicked(QPointF)), this, SLOT(clickedPoint(QPointF)));
  else 
    disconnect(mGraphicViewerWidget, SIGNAL(mouseClicked(QPointF)), this, SLOT(clickedPoint(QPointF)));
}

void GeoreferenceViewImp::setPoints(const std::list<std::pair<QString, QPointF>> &points)
{
  QBrush brush;
  brush = QBrush(Qt::NoBrush);

  QPen pen(QColor("#FF0000"), 1);
  pen.setCosmetic(true);
  QPen select_pen(QColor("#FF0000"), 2);
  select_pen.setCosmetic(true);

  for (auto &item : mGraphicViewerWidget->scene()->items()) {
    if (DiagonalCrossGraphicItem *keyPoint = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
      mGraphicViewerWidget->scene()->removeItem(item);
      delete keyPoint;
      keyPoint = nullptr;
    } else if (CrossGraphicItem *keyPoint = dynamic_cast<CrossGraphicItem *>(item)) {
      mGraphicViewerWidget->scene()->removeItem(item);
      delete keyPoint;
      keyPoint = nullptr;
    } else if (CircleGraphicItem *keyPoint = dynamic_cast<CircleGraphicItem *>(item)) {
      mGraphicViewerWidget->scene()->removeItem(item);
      delete keyPoint;
      keyPoint = nullptr;
    }
  }

  for (auto &point : points) {
    
    CrossGraphicItem *crossGraphicItemLeft = new CrossGraphicItem(point.second, 20);
    crossGraphicItemLeft->setPen(pen);
    crossGraphicItemLeft->setFlag(QGraphicsItem::ItemIsSelectable, true);
    crossGraphicItemLeft->setToolTip(point.first);
    crossGraphicItemLeft->setSelectedPen(select_pen);
    mGraphicViewerWidget->scene()->addItem(crossGraphicItemLeft);
  }
}

void GeoreferenceViewImp::setCrs(const QString &crs)
{
  mLineEditCRS->setText(crs);
}

void GeoreferenceViewImp::removeGroundControlPoints()
{
  QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
  QModelIndexList modelIndexList = selecctionModel->selectedRows();
  while(modelIndexList.size() > 0){
    int index = modelIndexList.at(0).row();
    emit removeGroundControlPoint(index);
    modelIndexList = selecctionModel->selectedRows();
  }
}

void GeoreferenceViewImp::initUI()
{
  this->setObjectName(QStringLiteral("ExportOrientationsView"));
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(400, 200);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mLabelCRS = new QLabel(this);
  gridLayout->addWidget(mLabelCRS, 0, 0, 1, 1);
  mLineEditCRS = new QLineEdit(this);
  mLineEditCRS->setMaximumWidth(250);
  gridLayout->addWidget(mLineEditCRS, 0, 1, 1, 1);

  QGridLayout *gridLayout2 = new QGridLayout();

  mTableViewGroundControlPoints = new QTableView(this);
  gridLayout2->addWidget(mTableViewGroundControlPoints, 1, 0, 4, 1);

  mPushButtonAddPoint = new QPushButton(this);
  mPushButtonAddPoint->setMaximumWidth(93);
  gridLayout2->addWidget(mPushButtonAddPoint, 1, 1, 1, 1);

  mPushButtonDeletePoint = new QPushButton(this);
  mPushButtonDeletePoint->setMaximumWidth(93);
  gridLayout2->addWidget(mPushButtonDeletePoint, 2, 1, 1, 1);

  mPushButtonGeoreference = new QPushButton(this);
  mPushButtonGeoreference->setMaximumWidth(93);
  gridLayout2->addWidget(mPushButtonGeoreference, 3, 1, 1, 1);

  verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  gridLayout2->addItem(verticalSpacer, 4, 1, 1, 1);


  //mLabelFilterByControlPoint = new QLabel(this);
  //gridLayout2->addWidget(mLabelFilterByControlPoint, 5, 0, 1, 1);
  //mComboBoxFilterByControlPoint = new QComboBox(this);
  //gridLayout2->addWidget(mComboBoxFilterByControlPoint, 5, 1, 1, 1);
  //mLabelFilterByImage = new QLabel(this);
  //gridLayout2->addWidget(mLabelFilterByImage, 6, 2, 1, 1);
  //mComboBoxFilterByImage = new QComboBox(this);
  //gridLayout2->addWidget(mComboBoxFilterByImage, 6, 3, 1, 1);

  mLabelImagePoints = new QLabel(this);
  gridLayout2->addWidget(mLabelImagePoints, 5, 0, 1, 1);

  mTableViewImagePoints = new QTableView(this);
  gridLayout2->addWidget(mTableViewImagePoints, 6, 0, 1, 1);

  gridLayout->addLayout(gridLayout2, 1, 0, 2, 2);

  mLabelImage = new QLabel(this);
  gridLayout->addWidget(mLabelImage, 1, 3, 1, 1);

  mComboBoxImages = new QComboBox(this);
  gridLayout->addWidget(mComboBoxImages, 1, 4, 1, 1);

  mGraphicViewerWidget = new GraphicViewerImp(this);
  mGraphicViewerWidget->setMinimumSize(QSize(541, 441));
  gridLayout->addWidget(mGraphicViewerWidget, 2, 2, 1, 3);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 3, 0, 1, 5);


  mContextMenuLeft = new QMenu(mGraphicViewerWidget);

  mActionZoomIn = new QAction(this);
  QIcon iconZoomIn;
  iconZoomIn.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomIn->setIcon(iconZoomIn);
  mContextMenuLeft->addAction(mActionZoomIn);

  mActionZoomOut = new QAction(this);
  QIcon iconZoomOut;
  iconZoomOut.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomOut->setIcon(iconZoomOut);
  mContextMenuLeft->addAction(mActionZoomOut);

  mActionZoomExtend = new QAction(this);
  QIcon iconZoomExtend;
  iconZoomExtend.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-magnifying-glass-with-expand-sign.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoomExtend->setIcon(iconZoomExtend);
  mContextMenuLeft->addAction(mActionZoomExtend);

  mActionZoom11 = new QAction(this);
  QIcon iconZoom11;
  iconZoom11.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-one-to-one.png"), QSize(), QIcon::Normal, QIcon::Off);
  mActionZoom11->setIcon(iconZoom11);
  mContextMenuLeft->addAction(mActionZoom11);

  mGraphicViewerWidget->setContextMenu(mContextMenuLeft);


  this->retranslate();
  this->update();
}

void GeoreferenceViewImp::initSignalAndSlots()
{
  connect(mComboBoxImages,   &QComboBox::currentTextChanged,      this,          &GeoreferenceView::imageChange);
  //connect(mTreeWidgetGroundControlPoints, &QTreeWidget::itemSelectionChanged, this, &GeoreferenceViewImp::onTreeWidgetItemSelectionChanged);
  connect(mGraphicViewerWidget, &GraphicViewer::selectionChanged, this,  &GeoreferenceViewImp::onGraphicsViewSelectionChanged);
  connect(mGraphicViewerWidget, &GraphicViewer::removeSelectItems, this,  &GeoreferenceViewImp::onGraphicsViewRemoveSelectItems);
  connect(mActionZoomIn,     SIGNAL(triggered(bool)), mGraphicViewerWidget, SLOT(zoomIn()));
  connect(mActionZoomOut,    SIGNAL(triggered(bool)), mGraphicViewerWidget, SLOT(zoomOut()));
  connect(mActionZoomExtend, SIGNAL(triggered(bool)), mGraphicViewerWidget, SLOT(zoomExtend()));
  connect(mActionZoom11,     SIGNAL(triggered(bool)), mGraphicViewerWidget, SLOT(zoom11()));
  connect(mPushButtonAddPoint,  &QAbstractButton::clicked, this, &GeoreferenceViewImp::addGroundControlPoint);
  connect(mPushButtonDeletePoint,  &QAbstractButton::clicked, this, &GeoreferenceViewImp::removeGroundControlPoints);
  connect(mPushButtonGeoreference,  &QAbstractButton::clicked, this, &GeoreferenceViewImp::georeference);

  connect(mLineEditCRS, &QLineEdit::textChanged,   this, &GeoreferenceView::crsChange);
  //connect(mPushButtonImportGroundControlPoints,  &QAbstractButton::clicked, this, &GeoreferenceViewImp::openFile);
  //connect(mLineEditCrsInput, &QLineEdit::textChanged, this, &GeoreferenceViewImp::update);
  //connect(mLineEditCrsOutput, &QLineEdit::textChanged, this, &GeoreferenceViewImp::update);

  connect(mButtonBox,                                   &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked,   this, &QDialog::accept);
  connect(mButtonBox->button(QDialogButtonBox::Help),   &QAbstractButton::clicked,   this, &DialogView::help);
}

void GeoreferenceViewImp::clear()
{

  //mLineEditImportCameras->clear();
  //mLineEditCrsInput->clear();
  //mLineEditCrsOutput->clear();
  //mProjectPath = "";

  update();
}

void GeoreferenceViewImp::update()
{
  
  if (QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel()) {
    QModelIndexList modelIndexList = selecctionModel->selectedRows();
    mPushButtonDeletePoint->setDisabled(modelIndexList.empty());
  } else {
    mPushButtonDeletePoint->setEnabled(false);
  }

  
  if (bSelectedItem == false){
    //if (QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel())
    //  selecctionModel->clearSelection();
  } else {
    /// Boton para borrar punto o puntos seleccionados
  }

  //QFileInfo file_info(mLineEditImportCameras->text());
  //bool file_exist = file_info.exists();

  //QPalette palette;
  //if(file_exist){
  //  palette.setColor(QPalette::Text, Qt::black);
  //} else {
  //  palette.setColor(QPalette::Text, Qt::red);
  //}
  //mLineEditImportCameras->setPalette(palette);

//  bool bx = mComboBoxXColumn->currentText().compare("--") != 0;
//  bool by = mComboBoxYColumn->currentText().compare("--") != 0;
//  bool bz = mComboBoxZColumn->currentText().compare("--") != 0;

//  mPushButtonGeoreference->setEnabled(file_exist && bx && by && bz);
}

void GeoreferenceViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
  //mPushButtonImportGroundControlPoints->setText(QCoreApplication::translate("GeoreferenceViewImp", "Import Ground Control Points", nullptr));
  mLabelCRS->setText(QCoreApplication::translate("GeoreferenceViewImp", "Coordinate Reference System:", nullptr));
  mPushButtonAddPoint->setText(QCoreApplication::translate("GeoreferenceViewImp", "Add", nullptr));
  mPushButtonDeletePoint->setText(QCoreApplication::translate("GeoreferenceViewImp", "Remove", nullptr));
  mPushButtonGeoreference->setText(QCoreApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
  mLabelImagePoints->setText(QCoreApplication::translate("GeoreferenceViewImp", "Image Points:", nullptr));
  mLabelImage->setText(QCoreApplication::translate("GeoreferenceViewImp", "Image:", nullptr));
  mActionZoomIn->setText(QApplication::translate("FeaturesViewerView", "Zoom In"));
  mActionZoomOut->setText(QApplication::translate("FeaturesViewerView", "Zoom Out"));
  mActionZoomExtend->setText(QApplication::translate("FeaturesViewerView", "Zoom Extend"));
  mActionZoom11->setText(QApplication::translate("FeaturesViewerView", "Zoom 1:1"));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("GeoreferenceViewImp", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("GeoreferenceViewImp", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("GeoreferenceViewImp", "Help", nullptr));
}

void GeoreferenceViewImp::openFile()
{
//  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
//                                              tr("Ground Control Points"),
//                                              mProjectPath,
//                                              tr("Comma-separated values (*.csv); XYZ (*.txt)"));
//  if (!file.isEmpty()){
//    mLineEditImportCameras->setText(file);
//
//    emit loadCSV(file, ",");
//  }
}

void GeoreferenceViewImp::setProjectPath(const QString &path)
{
  mProjectPath = path;
}

//QString GeoreferenceViewImp::orientationFile() const
//{
//  return mLineEditImportCameras->text();
//}

//void GeoreferenceViewImp::setTableHeader(const QStringList &header)
//{
//  QStandardItemModel *model = dynamic_cast<QStandardItemModel *>(mTableViewGroundControlPoints->model());
//  model->setColumnCount(header.size());
//  model->setHorizontalHeaderLabels(header);
//}

//void GeoreferenceViewImp::setItemModel(QStandardItemModel *model)
//{
//  mTableViewGroundControlPoints->setModel(model);
//}

} // namespace graphos


