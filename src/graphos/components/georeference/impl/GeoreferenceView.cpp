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

#include "GeoreferenceView.h"

#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/GraphicItem.h"
#include "graphos/core/utils.h"

#include <tidop/img/imgreader.h>

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTableView>
#include <QFileDialog>
#include <QComboBox>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QToolBar>
#include <QFileDialog>
#include <QStandardPaths>


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

    QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
    bSelectedItem = false;
    for (int i = 0; i < items.size(); i++) {
        if (items[i] && items[i]->isSelected() == true) {

            bSelectedItem = true;
            break;
        }
    }

    update();
}

void GeoreferenceViewImp::onGraphicsViewRemoveSelectItems()
{
    const QSignalBlocker blocker1(mGraphicViewerWidget);
    QString image = mComboBoxImages->currentText();

    QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
    for (int i = 0; i < items.size(); i++) {
        if (items[i] && items[i]->isSelected() == true) {
            QString gcp = items[i]->toolTip();
            mGraphicViewerWidget->scene()->removeItem(items[i]);
            emit remove_image_point(gcp, items[i]->data(Qt::UserRole).toULongLong());
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

    // for (auto modelIndex : modelIndexList) {
    //   // Se obtiene el identificador de la fila seleccionada y se filtran los puntos imagen.
    //   int index = modelIndex.row();
    //   //mTableViewGroundControlPoints->
    // }

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

        emit add_image_point(groundControlPoint, mComboBoxImages->currentData().toULongLong(), point);

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

QString GeoreferenceViewImp::crs() const
{
    return mLineEditCRS->text();
}

void GeoreferenceViewImp::setImageList(const std::vector<std::pair<size_t, QString>> &imageList)
{
    QSignalBlocker blocker(mComboBoxImages);
    mComboBoxImages->clear();
    for (auto &image : imageList) {
        QFileInfo file_info(image.second);
        mComboBoxImages->addItem(file_info.baseName(), static_cast<qulonglong>(image.first));
    }
}

void GeoreferenceViewImp::setCurrentImage(const QString &imagePath)
{
    QSignalBlocker blocker(mComboBoxImages);
    QFileInfo file_info(imagePath);
    mComboBoxImages->setCurrentText(file_info.baseName());
    mGraphicViewerWidget->scene()->clearSelection();

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(imagePath.toStdString());
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

void GeoreferenceViewImp::importGCP()
{
    QString selectedFilter;
    QString pathName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Import Ground Control Points"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Open Drone Map (*.txt)"),
                                                    &selectedFilter);

    if (!pathName.isEmpty()) {
        QString format;
        if (selectedFilter.compare("Open Drone Map (*.txt)") == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit import_gcp(pathName, format);
    }
}

void GeoreferenceViewImp::exportGCP()
{
    QString selectedFilter;
    QString pathName = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Export Ground Control Points"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Open Drone Map (*.txt)"),
                                                    &selectedFilter);

    if (!pathName.isEmpty()) {

        QString format;
        if (selectedFilter.compare("Open Drone Map (*.txt)") == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit export_gcp(pathName, format);
    }
}

void GeoreferenceViewImp::removeGroundControlPoints()
{
    QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel();
    QModelIndexList modelIndexList = selecctionModel->selectedRows();
    while (modelIndexList.size() > 0) {
        int index = modelIndexList.at(0).row();
        emit removeGroundControlPoint(index);
        modelIndexList = selecctionModel->selectedRows();
    }
}

void GeoreferenceViewImp::initUI()
{
    this->setObjectName(QStringLiteral("ExportOrientationsView"));
    this->resize(400, 200);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    QToolBar *toolBar = new QToolBar(this);
    mActionImportGCP = new QAction(this);
    mActionImportGCP->setIcon(QIcon::fromTheme("import"));
    toolBar->addAction(mActionImportGCP);

    mActionExportGCP = new QAction(this);
    mActionExportGCP->setIcon(QIcon::fromTheme("export"));
    toolBar->addAction(mActionExportGCP);

    toolBar->addSeparator();

    mActionAddPoint = new QAction(this);
    //mActionAddPoint->setIcon(QIcon::fromTheme("save"));
    toolBar->addAction(mActionAddPoint);

    mActionDeletePoint = new QAction(this);
    //mActionDeletePoint->setIcon(QIcon::fromTheme("save"));
    toolBar->addAction(mActionDeletePoint);

    toolBar->addSeparator();

    mActionGeoreference = new QAction(this);
    //mActionGeoreference->setIcon(QIcon::fromTheme("save"));
    toolBar->addAction(mActionGeoreference);

    gridLayout->addWidget(toolBar, 0, 0, 1, 2);

    mLabelCRS = new QLabel(this);
    gridLayout->addWidget(mLabelCRS, 1, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    mLineEditCRS->setMaximumWidth(250);
    gridLayout->addWidget(mLineEditCRS, 1, 1, 1, 1);

    QGridLayout *gridLayout2 = new QGridLayout();

    mTableViewGroundControlPoints = new QTableView(this);
    gridLayout2->addWidget(mTableViewGroundControlPoints, 0, 0, 4, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(verticalSpacer, 3, 1, 1, 1);

    mLabelImagePoints = new QLabel(this);
    gridLayout2->addWidget(mLabelImagePoints, 4, 0, 1, 1);

    mTableViewImagePoints = new QTableView(this);
    gridLayout2->addWidget(mTableViewImagePoints, 5, 0, 1, 1);

    gridLayout->addLayout(gridLayout2, 2, 0, 2, 2);

    mLabelImage = new QLabel(this);
    gridLayout->addWidget(mLabelImage, 1, 3, 1, 1);

    mComboBoxImages = new QComboBox(this);
    gridLayout->addWidget(mComboBoxImages, 1, 4, 1, 1);

    mGraphicViewerWidget = new GraphicViewer(this);
    mGraphicViewerWidget->setMinimumSize(QSize(541, 441));
    gridLayout->addWidget(mGraphicViewerWidget, 2, 2, 1, 3);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 3, 0, 1, 5);

    this->retranslate();
    this->update();
}

void GeoreferenceViewImp::initSignalAndSlots()
{
    connect(mComboBoxImages, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [&](int idx) {
                emit image_changed(mComboBoxImages->itemData(idx).toULongLong());
            });
    
    connect(mGraphicViewerWidget, &GraphicViewer::selectionChanged, this, &GeoreferenceViewImp::onGraphicsViewSelectionChanged);
    connect(mGraphicViewerWidget, &GraphicViewer::removeSelectItems, this, &GeoreferenceViewImp::onGraphicsViewRemoveSelectItems);
    connect(mActionImportGCP, &QAction::triggered, this, &GeoreferenceViewImp::importGCP);
    connect(mActionExportGCP, &QAction::triggered, this, &GeoreferenceViewImp::exportGCP);
    connect(mActionAddPoint, &QAction::triggered, this, &GeoreferenceViewImp::addGroundControlPoint);
    connect(mActionDeletePoint, &QAction::triggered, this, &GeoreferenceViewImp::removeGroundControlPoints);
    connect(mActionGeoreference, &QAction::triggered, this, &GeoreferenceViewImp::georeference);

    connect(mLineEditCRS, &QLineEdit::textChanged, this, &GeoreferenceView::crsChange);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &QDialog::accept);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void GeoreferenceViewImp::clear()
{

    update();
}

void GeoreferenceViewImp::update()
{

    if (QItemSelectionModel *selecctionModel = mTableViewGroundControlPoints->selectionModel()) {
        QModelIndexList modelIndexList = selecctionModel->selectedRows();
        mActionDeletePoint->setDisabled(modelIndexList.empty());
    } else {
        mActionDeletePoint->setEnabled(false);
    }


    if (bSelectedItem == false) {
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

  //  mActionGeoreference->setEnabled(file_exist && bx && by && bz);
}

void GeoreferenceViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
    //mPushButtonImportGroundControlPoints->setText(QCoreApplication::translate("GeoreferenceViewImp", "Import Ground Control Points", nullptr));
    mLabelCRS->setText(QCoreApplication::translate("GeoreferenceViewImp", "Coordinate Reference System:", nullptr));
    mActionAddPoint->setText(QCoreApplication::translate("GeoreferenceViewImp", "Add point", nullptr));
    mActionDeletePoint->setText(QCoreApplication::translate("GeoreferenceViewImp", "Remove", nullptr));
    mActionGeoreference->setText(QCoreApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
    mLabelImagePoints->setText(QCoreApplication::translate("GeoreferenceViewImp", "Image Points:", nullptr));
    mLabelImage->setText(QCoreApplication::translate("GeoreferenceViewImp", "Image:", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("GeoreferenceViewImp", "Cancel", nullptr));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("GeoreferenceViewImp", "Save", nullptr));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("GeoreferenceViewImp", "Help", nullptr));
}

//void GeoreferenceViewImp::openFile()
//{
//    //  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
//    //                                              tr("Ground Control Points"),
//    //                                              mProjectPath,
//    //                                              tr("Comma-separated values (*.csv); XYZ (*.txt)"));
//    //  if (!file.isEmpty()){
//    //    mLineEditImportCameras->setText(file);
//    //
//    //    emit loadCSV(file, ",");
//    //  }
//}

void GeoreferenceViewImp::setProjectPath(const QString &path)
{
    mProjectPath = path;
}

} // namespace graphos


