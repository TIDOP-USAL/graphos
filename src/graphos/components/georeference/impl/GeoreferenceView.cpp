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
    GeoreferenceViewImp::initUI();
    GeoreferenceViewImp::initSignalAndSlots();
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
    QItemSelectionModel *selecction_model = mTableViewGroundControlPoints->selectionModel();

    QModelIndexList model_index_list = selecction_model->selectedRows();

    /// Por ahora...
    this->setEnableImagePointsAddOrEdit(model_index_list.size() == 1);

    // for (auto modelIndex : modelIndexList) {
    //   // Se obtiene el identificador de la fila seleccionada y se filtran los puntos imagen.
    //   int index = modelIndex.row();
    //   //mTableViewGroundControlPoints->
    // }

    this->update();
}

void GeoreferenceViewImp::clickedPoint(const QPointF &point)
{

    QItemSelectionModel *selecction_model = mTableViewGroundControlPoints->selectionModel();
    QModelIndexList model_index_list = selecction_model->selectedRows();

    int index = -1;
    if (model_index_list.size() == 1) {
        index = model_index_list.at(0).row();

    } else {
        /// Si no hay nada activado mostrar lista de Ground Control Points para seleccionar
    }

    if (index != -1) {
        QString ground_control_point = mTableViewGroundControlPoints->model()->index(index, 0).data().toString();

        emit add_image_point(ground_control_point, mComboBoxImages->currentData().toULongLong(), point);

        QPen pen(QColor("#FF0000"), 1);
        pen.setCosmetic(true);
        QPen select_pen(QColor("#FF0000"), 2);
        select_pen.setCosmetic(true);

        QList<QGraphicsItem *> items = mGraphicViewerWidget->items();
        for (const auto &item : items) {
            if (item->toolTip().compare(ground_control_point) == 0) {
                mGraphicViewerWidget->scene()->removeItem(item);
                break;
            }
        }

        auto cross_graphic_item_left = new CrossGraphicItem(point, 20);
        cross_graphic_item_left->setPen(pen);
        cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
        cross_graphic_item_left->setToolTip(ground_control_point);
        cross_graphic_item_left->setSelectedPen(select_pen);
        mGraphicViewerWidget->scene()->addItem(cross_graphic_item_left);
    }
}

auto GeoreferenceViewImp::crs() const -> QString
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

    auto image_reader = tl::ImageReaderFactory::create(imagePath.toStdString());
    image_reader->open();
    if (image_reader->isOpen()) {
        cv::Mat bmp = image_reader->read();
        mGraphicViewerWidget->setImage(cvMatToQImage(bmp));
        image_reader->close();
    }
    mGraphicViewerWidget->zoomExtend();
}

void GeoreferenceViewImp::setItemModelGroundControlPoints(QAbstractItemModel *model)
{
    mTableViewGroundControlPoints->setModel(model);
    QItemSelectionModel *selecction_model = mTableViewGroundControlPoints->selectionModel();
    connect(selecction_model, &QItemSelectionModel::selectionChanged, this, &GeoreferenceViewImp::selectionChanged);
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
    //QBrush brush(Qt::NoBrush);

    QPen pen(QColor("#FF0000"), 1);
    pen.setCosmetic(true);
    QPen select_pen(QColor("#FF0000"), 2);
    select_pen.setCosmetic(true);

    for (auto &item : mGraphicViewerWidget->scene()->items()) {
        if (auto key_point = dynamic_cast<DiagonalCrossGraphicItem *>(item)) {
            mGraphicViewerWidget->scene()->removeItem(item);
            delete key_point;
            key_point = nullptr;
        } else if (auto key_point = dynamic_cast<CrossGraphicItem *>(item)) {
            mGraphicViewerWidget->scene()->removeItem(item);
            delete key_point;
            key_point = nullptr;
        } else if (auto key_point = dynamic_cast<CircleGraphicItem *>(item)) {
            mGraphicViewerWidget->scene()->removeItem(item);
            delete key_point;
            key_point = nullptr;
        }
    }

    for (auto &point : points) {
        auto cross_graphic_item_left = new CrossGraphicItem(point.second, 20);
        cross_graphic_item_left->setPen(pen);
        cross_graphic_item_left->setFlag(QGraphicsItem::ItemIsSelectable, true);
        cross_graphic_item_left->setToolTip(point.first);
        cross_graphic_item_left->setSelectedPen(select_pen);
        mGraphicViewerWidget->scene()->addItem(cross_graphic_item_left);
    }
}

void GeoreferenceViewImp::setCrs(const QString &crs)
{
    mLineEditCRS->setText(crs);
}

void GeoreferenceViewImp::importGCP()
{
    QString selected_filter;
    QString path = QFileDialog::getOpenFileName(nullptr,
                                                tr("Import Ground Control Points"),
                                                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                tr("Open Drone Map (*.txt)"),
                                                &selected_filter);

    if (!path.isEmpty()) {
        QString format;
        if (selected_filter.compare("Open Drone Map (*.txt)") == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit import_gcp(path, format);
    }
}

void GeoreferenceViewImp::exportGCP()
{
    QString selected_filter;
    QString path = QFileDialog::getSaveFileName(nullptr,
                                                tr("Export Ground Control Points"),
                                                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                tr("Open Drone Map (*.txt)"),
                                                &selected_filter);

    if (!path.isEmpty()) {

        QString format;
        if (selected_filter.compare("Open Drone Map (*.txt)") == 0) {
            format = "ODM";
        } else {
            tl::Message::error("Unsupported format");
        }

        emit export_gcp(path, format);
    }
}

void GeoreferenceViewImp::removeGroundControlPoints()
{
    QItemSelectionModel *selecction_model = mTableViewGroundControlPoints->selectionModel();
    QModelIndexList model_index_list = selecction_model->selectedRows();
    while (!model_index_list.empty()) {
        int index = model_index_list.at(0).row();
        emit removeGroundControlPoint(index);
        model_index_list = selecction_model->selectedRows();
    }
}

void GeoreferenceViewImp::initUI()
{
    this->setObjectName(QStringLiteral("ExportOrientationsView"));
    this->resize(400, 200);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    auto tool_bar = new QToolBar(this);
    mActionImportGCP = new QAction(this);
    mActionImportGCP->setIcon(QIcon::fromTheme("import"));
    tool_bar->addAction(mActionImportGCP);

    mActionExportGCP = new QAction(this);
    mActionExportGCP->setIcon(QIcon::fromTheme("export"));
    tool_bar->addAction(mActionExportGCP);

    tool_bar->addSeparator();

    mActionAddPoint = new QAction(this);
    tool_bar->addAction(mActionAddPoint);

    mActionDeletePoint = new QAction(this);
    tool_bar->addAction(mActionDeletePoint);

    tool_bar->addSeparator();

    mActionGeoreference = new QAction(this);
    tool_bar->addAction(mActionGeoreference);

    grid_layout->addWidget(tool_bar, 0, 0, 1, 5);

    mLabelCRS = new QLabel(this);
    grid_layout->addWidget(mLabelCRS, 1, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    mLineEditCRS->setMaximumWidth(200);
    mLineEditCRS->setDisabled(true);
    grid_layout->addWidget(mLineEditCRS, 1, 1, 1, 1);
    mQPushButtonCRS = new QPushButton(this);
    mQPushButtonCRS->setMaximumSize(QSize(31, 28));
    mQPushButtonCRS->setText("...");
    grid_layout->addWidget(mQPushButtonCRS, 1, 2, 1, 1);

    auto grid_layout2 = new QGridLayout();

    mTableViewGroundControlPoints = new QTableView(this);
    grid_layout2->addWidget(mTableViewGroundControlPoints, 0, 0, 4, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    grid_layout2->addItem(verticalSpacer, 3, 1, 1, 1);

    mLabelImagePoints = new QLabel(this);
    grid_layout2->addWidget(mLabelImagePoints, 4, 0, 1, 1);

    mTableViewImagePoints = new QTableView(this);
    grid_layout2->addWidget(mTableViewImagePoints, 5, 0, 1, 1);

    grid_layout->addLayout(grid_layout2, 2, 0, 2, 3);

    mLabelImage = new QLabel(this);
    grid_layout->addWidget(mLabelImage, 1, 4, 1, 1);

    mComboBoxImages = new QComboBox(this);
    grid_layout->addWidget(mComboBoxImages, 1, 5, 1, 1);

    mGraphicViewerWidget = new GraphicViewer(this);
    mGraphicViewerWidget->setMinimumSize(QSize(541, 441));
    grid_layout->addWidget(mGraphicViewerWidget, 2, 3, 1, 3);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 4, 0, 1, 6);

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
    connect(mQPushButtonCRS, &QAbstractButton::clicked, this, &GeoreferenceView::select_crs);

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
}

void GeoreferenceViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("GeoreferenceViewImp", "Georeference", nullptr));
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

void GeoreferenceViewImp::setProjectPath(const QString &path)
{
    mProjectPath = path;
}

} // namespace graphos


