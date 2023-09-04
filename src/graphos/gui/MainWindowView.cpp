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

#include "MainWindowView.h"
#include "ui_MainWindowView.h"

#include "graphos/widgets/ThumbnailsWidget.h"
#include "graphos/widgets/LogWidget.h"
#include "graphos/widgets/TabWidget.h"
#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/Viewer3d.h"
#include "graphos/widgets/ProgressBarWidget.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <tidop/core/msg/message.h>

#include <QHeaderView>
#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QSettings>
#include <QGraphicsEllipseItem>
#include <QDesktopServices>
#include <QUrl>
#include <QComboBox>
#include <QProgressBar>
#include <QLabel>
#include <QToolBar>

#include <unordered_map>

namespace graphos
{

enum
{
    project,
    images,
    image,                   // Imagen
    image_features,          // Imagen + features
    image_features_matches,  // Imagen + features + matches
    //  features,
    //  features_images,
    //  features_image,
    detector,
    descriptor,
    matches,
    descriptor_matcher,
    matches_pairs,
    pair_left,
    pair_right,
    sparse_model,
    dense_model,
    mesh,
    dsm,
    ortho
};

MainWindowView::MainWindowView(QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindowView),
    mActionNewProject(new QAction(this)),
    mActionOpenProject(new QAction(this)),
    mActionSaveProject(new QAction(this)),
    mActionSaveProjectAs(new QAction(this)),
    mActionCloseProject(new QAction(this)),
    mActionExit(new QAction(this)),
    mActionStartPage(new QAction(this)),
    mActionCameras(new QAction(this)),
    mActionHelp(new QAction(this)),
    mActionImportCameras(new QAction(this)),
    mActionExportCameras(new QAction(this)),
    mActionExportTiePoints(new QAction(this)),
    mActionExportMatches(new QAction(this)),
    mActionExportPointCloud(new QAction(this)),
    mActionOrtho(new QAction(this)),
    mActionOpenImage(new QAction(this)),
    mActionDeleteImage(new QAction(this)),
    mActionViewKeypoints(new QAction(this)),
    mActionViewMatches(new QAction(this)),
    mActionOpenModel3D(new QAction(this)),
    mActionZoomIn(new QAction(this)),
    mActionZoomOut(new QAction(this)),
    mActionZoomExtend(new QAction(this)),
    mActionZoom11(new QAction(this)),
    mActionGlobalZoom(new QAction(this)),
    mActionViewFront(new QAction(this)),
    mActionViewTop(new QAction(this)),
    mActionViewLeft(new QAction(this)),
    mActionViewRight(new QAction(this)),
    mActionViewBack(new QAction(this)),
    mActionViewBottom(new QAction(this)),
    mActionPointMeasuse(new QAction(this)),
    mActionDistanceMeasuse(new QAction(this)),
    mActionAngleMeasure(new QAction(this))
{
    ui->setupUi(this);

    this->initUI();
    this->initSignalAndSlots();

    Application &app = Application::instance();
    AppStatus *app_status = app.status();
    connect(app_status, &AppStatus::update,
            this, &MainWindowView::update);
}

MainWindowView::~MainWindowView()
{
    delete ui;
}

void MainWindowView::addActionToMenu(QAction *action,
                                     Menu menuName)
{
    if (QMenu *_menu = findMenu(menuName)) {

        QAction *prev_action = nullptr;
        for (auto _action : _menu->actions()) {
            if (_action->text() == action->text()) {
                prev_action = _action;
                break;
            }
        }

        if (prev_action) {
            _menu->insertAction(prev_action, action);
            _menu->removeAction(prev_action);
        } else {
            _menu->addAction(action);
        }

    }
}

void MainWindowView::addMenuToMenu(QMenu *menu, Menu menuName)
{
    if (QMenu *_menu = findMenu(menuName)) {

        QMenu *sub_menu = nullptr;
        for (auto _action : _menu->actions()) {
            if (QMenu *_sub_menu = _action->menu()) {
                if (_sub_menu->title() == menu->title()) {
                    sub_menu = _sub_menu;
                    break;
                }
            }
        }

        if (sub_menu) {
            _menu->insertMenu(sub_menu->menuAction(), menu);
            _menu->removeAction(sub_menu->menuAction());
        } else {
            _menu->addMenu(menu);
        }

    }
}

void MainWindowView::addSeparatorToMenu(Menu menu)
{
    if (QMenu *_menu = findMenu(menu)) {
        _menu->addSeparator();
    }
}

void MainWindowView::addActionToToolbar(QAction *action,
                                        Toolbar toolbar)
{
    if (QToolBar *_toolbar = findToolbar(toolbar)) {

        QAction *prev_action = nullptr;
        for (auto _action : _toolbar->actions()) {
            if (_action->text() == action->text()) {
                prev_action = _action;
                break;
            }
        }

        if (prev_action) {
            _toolbar->insertAction(prev_action, action);
            _toolbar->removeAction(prev_action);
        } else {
            _toolbar->addAction(action);
        }

    }
}

void MainWindowView::addSeparatorToToolbar(Toolbar toolbar)
{
    if (QToolBar *_toolbar = findToolbar(toolbar)) {
        _toolbar->addSeparator();
    }
}

void MainWindowView::setPropertiesWidget(QWidget *widget)
{
    ui->dockWidgetProperties->toggleViewAction()->setIcon(QIcon::fromTheme("properties"));
    ui->dockWidgetProperties->setWidget(widget);
}

void MainWindowView::clear()
{
    setWindowTitle(QString(""));
    const QSignalBlocker blockerTreeWidgetProject(mTreeWidgetProject);
    mTreeWidgetProject->clear();
    const QSignalBlocker blockerThumbnailsWidget(mThumbnailsWidget);
    mThumbnailsWidget->clear();

    if (mTabWidget) mTabWidget->clear();

    update();
}

void MainWindowView::setProjectTitle(const QString &title)
{
    this->setWindowTitle(title);

    QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0);
    if (itemProject == nullptr) {
        itemProject = new QTreeWidgetItem();
        mTreeWidgetProject->addTopLevelItem(itemProject);
        itemProject->setExpanded(true);
        itemProject->setData(0, Qt::UserRole, graphos::project);
    }

    itemProject->setText(0, tr("Project: ").append(title));
}

void MainWindowView::addImage(const Image &image, const Camera &camera)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages == nullptr) {
            itemImages = new QTreeWidgetItem();
            itemImages->setText(0, tr("Images"));
            itemImages->setIcon(0, QIcon::fromTheme("pictures-folder"));
            itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
            itemImages->setData(0, Qt::UserRole, graphos::images);
            itemProject->addChild(itemImages);
            itemImages->setExpanded(true);
        }

        /* Se añade el fotograma al árbol del proyecto */
        QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
        itemPhotogram->setText(0, image.name());
        itemPhotogram->setIcon(0, QIcon::fromTheme("image-file"));
        itemPhotogram->setToolTip(0, image.path());
        itemPhotogram->setData(0, Qt::UserRole, graphos::image);
        itemPhotogram->setData(0, Qt::UserRole + 1, static_cast<qulonglong>(image.id()));
        itemImages->addChild(itemPhotogram);
        itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));

        update();
    }

    mThumbnailsWidget->addThumbnail(image, QSize(camera.width(), camera.height()));
}

void MainWindowView::addImages(const std::unordered_map<size_t, Image> &images,
                               const std::map<int, Camera> &cameras)
{

    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages == nullptr) {
            itemImages = new QTreeWidgetItem();
            itemImages->setText(0, tr("Images"));
            itemImages->setIcon(0, QIcon::fromTheme("pictures-folder"));
            itemImages->setFlags(itemImages->flags() | Qt::ItemIsTristate);
            itemImages->setData(0, Qt::UserRole, graphos::images);
            itemProject->addChild(itemImages);
            itemImages->setExpanded(true);
        }

        for (const auto &image : images) {

            size_t image_id = image.first;
            image.second.name();

            /* Se añade el fotograma al árbol del proyecto */
            QTreeWidgetItem *itemPhotogram = new QTreeWidgetItem();
            itemPhotogram->setText(0, image.second.name());
            itemPhotogram->setIcon(0, QIcon::fromTheme("image-file"));
            itemPhotogram->setToolTip(0, image.second.path());
            itemPhotogram->setData(0, Qt::UserRole, graphos::image);
            itemPhotogram->setData(0, Qt::UserRole + 1, static_cast<qulonglong>(image_id));
            itemImages->addChild(itemPhotogram);

            const auto &camera = cameras.find(image.second.cameraId());
            mThumbnailsWidget->addThumbnail(image.second, QSize(camera->second.width(), camera->second.height()));
        }

        itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));

        update();
    }

}

void MainWindowView::setActiveImage(size_t imageId)
{
    const QSignalBlocker blocker1(mTreeWidgetProject);
    const QSignalBlocker blocker2(mThumbnailsWidget);

    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                size_t id = temp->data(0, Qt::UserRole + 1).toULongLong();
                itemImages->child(i)->setSelected(id == imageId);
            }
        }
    }

    mThumbnailsWidget->setActiveImage(imageId);

}

void MainWindowView::setActiveImages(const std::vector<size_t> &imageIds)
{
    const QSignalBlocker blocker1(mTreeWidgetProject);
    const QSignalBlocker blocker2(mThumbnailsWidget);

    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                temp->setSelected(false);
                for (auto id : imageIds) {
                    if (temp->data(0, Qt::UserRole + 1).toULongLong() == id) {
                        temp->setSelected(true);
                        break;
                    }
                }
            }
        }
    }

    mThumbnailsWidget->setActiveImages(imageIds);
}

void MainWindowView::addFeatures(size_t imageId)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            QTreeWidgetItem *itemImage = nullptr;
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
                    itemImage = itemImages->child(i);
                    break;
                }
            }
            if (itemImage)
                itemImage->setData(0, Qt::UserRole, graphos::image_features);
        }
    }
}

void MainWindowView::deleteFeatures(size_t imageId)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            QTreeWidgetItem *itemImage = nullptr;
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
                    itemImage = itemImages->child(i);
                    break;
                }
            }
            if (itemImage)
                itemImage->setData(0, Qt::UserRole, graphos::image);
        }
    }
}

void MainWindowView::addMatches(size_t imageId)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            //if (temp->text(0).compare(tr("Images")) == 0) {
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            QTreeWidgetItem *itemImage = nullptr;
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
                    itemImage = itemImages->child(i);
                    break;
                }
            }

            itemImage->setData(0, Qt::UserRole, graphos::image_features_matches);
        }
    }
}

void MainWindowView::deleteMatches(size_t imageId)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {
            // Se busca la imagen en el árbol
            QTreeWidgetItem *itemImage = nullptr;
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                if (temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
                    itemImage = itemImages->child(i);
                    break;
                }
            }

            itemImage->setData(0, Qt::UserRole, graphos::image_features);
        }
    }
}

void MainWindowView::setSparseModel(const QString &sparseModel)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) {
            itemModels = new QTreeWidgetItem();
            itemModels->setText(0, tr("3D Models"));
            itemModels->setIcon(0, QIcon::fromTheme("folder_3d"));
            itemProject->addChild(itemModels);
            itemModels->setExpanded(true);
        }

        QTreeWidgetItem *itemSparseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Sparse Model")) == 0) {
                itemSparseModel = temp;
                break;
            }
        }

        if (itemSparseModel == nullptr) {
            itemSparseModel = new QTreeWidgetItem();
            itemModels->addChild(itemSparseModel);
        }

        itemSparseModel->setText(0, "Sparse Model");
        itemSparseModel->setIcon(0, QIcon::fromTheme("dense"));
        itemSparseModel->setToolTip(0, sparseModel);
        itemSparseModel->setData(0, Qt::UserRole, graphos::sparse_model);
    }
}

void MainWindowView::deleteSparseModel()
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) return;/*{
          itemModels = new QTreeWidgetItem();
          itemModels->setText(0, tr("3D Models"));
          itemProject->addChild(itemModels);
          itemModels->setExpanded(true);
        }*/

        QTreeWidgetItem *itemSparseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Sparse Model")) == 0) {
                itemSparseModel = temp;
                delete itemSparseModel;
                itemSparseModel = nullptr;
                break;
            }
        }
    }
}

void MainWindowView::setDenseModel(const QString &denseModel)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) {
            itemModels = new QTreeWidgetItem();
            itemModels->setText(0, tr("3D Models"));
            itemModels->setIcon(0, QIcon::fromTheme("folder_3d"));
            itemProject->addChild(itemModels);
            itemModels->setExpanded(true);
        }

        QTreeWidgetItem *itemDenseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Dense Model")) == 0) {
                itemDenseModel = temp;
                break;
            }
        }

        if (itemDenseModel == nullptr) {
            itemDenseModel = new QTreeWidgetItem();
            itemModels->addChild(itemDenseModel);
        }

        itemDenseModel->setText(0, "Dense Model");
        itemDenseModel->setIcon(0, QIcon::fromTheme("dense"));
        itemDenseModel->setToolTip(0, denseModel);
        itemDenseModel->setData(0, Qt::UserRole, graphos::dense_model);
    }
}

void MainWindowView::deleteDenseModel()
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) return;

        QTreeWidgetItem *itemDenseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Dense Model")) == 0) {
                itemDenseModel = temp;
                delete itemDenseModel;
                itemDenseModel = nullptr;
                break;
            }
        }

    }
}

void MainWindowView::setMesh(const QString &mesh)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) {
            itemModels = new QTreeWidgetItem();
            itemModels->setText(0, tr("3D Models"));
            itemModels->setIcon(0, QIcon::fromTheme("folder_3d"));
            itemProject->addChild(itemModels);
            itemModels->setExpanded(true);
        }

        QTreeWidgetItem *itemDenseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Mesh")) == 0) {
                itemDenseModel = temp;
                break;
            }
        }

        if (itemDenseModel == nullptr) {
            itemDenseModel = new QTreeWidgetItem();
            itemModels->addChild(itemDenseModel);
        }

        itemDenseModel->setText(0, "Mesh");
        itemDenseModel->setIcon(0, QIcon::fromTheme("mesh"));
        itemDenseModel->setToolTip(0, mesh);
        itemDenseModel->setData(0, Qt::UserRole, graphos::mesh);
    }
}

void MainWindowView::deleteMesh()
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemModels = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("3D Models")) == 0) {
                itemModels = itemProject->child(i);
                break;
            }
        }

        if (itemModels == nullptr) return;

        QTreeWidgetItem *itemDenseModel = nullptr;
        for (int i = 0; i < itemModels->childCount(); i++) {
            QTreeWidgetItem *temp = itemModels->child(i);
            if (temp->text(0).compare(tr("Mesh")) == 0) {
                itemDenseModel = temp;
                delete itemDenseModel;
                itemDenseModel = nullptr;
                break;
            }
        }

    }
}

void MainWindowView::setDSM(const QString &dsm)
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemDSM = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("DTM/DSM")) == 0) {
                itemDSM = temp;
                break;
            }
        }

        if (itemDSM == nullptr) {
            itemDSM = new QTreeWidgetItem();
            itemProject->addChild(itemDSM);
        }

        itemDSM->setText(0, "DTM/DSM");
        itemDSM->setIcon(0, QIcon::fromTheme("image-file"));
        itemDSM->setToolTip(0, dsm);
        itemDSM->setData(0, Qt::UserRole, graphos::dsm);
    }
}

void MainWindowView::deleteDsm()
{
    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemDSM = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->text(0).compare(tr("DTM/DSM")) == 0) {
                itemDSM = temp;
                delete itemDSM;
                itemDSM = nullptr;
                break;
            }
        }
    }
}

void MainWindowView::setOrtho(const QString& ortho)
{
}

void MainWindowView::deleteOrtho()
{
}

void MainWindowView::setStatusBarMsg(const QString &msg)
{
    ui->statusBar->showMessage(msg, 2000);
}

ProgressBarWidget *MainWindowView::progressBar()
{
    return mProgressBar;
}


TabWidget *MainWindowView::tabWidget()
{
    return mTabWidget;
}

void MainWindowView::deleteImages(const std::vector<size_t> &imageIds)
{
    const QSignalBlocker blocker1(mTreeWidgetProject);
    const QSignalBlocker blocker2(mThumbnailsWidget);

    if (QTreeWidgetItem *itemProject = mTreeWidgetProject->topLevelItem(0)) {

        QTreeWidgetItem *itemImages = nullptr;
        for (int i = 0; i < itemProject->childCount(); i++) {
            QTreeWidgetItem *temp = itemProject->child(i);
            if (temp->data(0, Qt::UserRole) == graphos::images) {
                itemImages = itemProject->child(i);
                break;
            }
        }

        if (itemImages) {

            // Se busca la imagen en el árbol
            QTreeWidgetItem *itemImage = nullptr;
            for (int i = 0; i < itemImages->childCount(); i++) {
                QTreeWidgetItem *temp = itemImages->child(i);
                for (auto imageId : imageIds) {
                    if (temp && temp->data(0, Qt::UserRole + 1).toULongLong() == imageId) {
                        itemImage = itemImages->child(i);
                        delete itemImage;
                        itemImage = nullptr;
                        i--;
                        break;
                    }
                }
            }

            itemImages->setText(0, tr("Images").append(" [").append(QString::number(itemImages->childCount())).append("]"));
        }

    }

    mThumbnailsWidget->deleteImages(imageIds);
}

void MainWindowView::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        this->retranslate();
        break;
    default:
        break;
    }
}



void MainWindowView::openFromHistory()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        emit openProjectFromHistory(action->data().toString());
}

void MainWindowView::onSelectionChanged()
{
    QList<QTreeWidgetItem *> item = mTreeWidgetProject->selectedItems();
    if (item.isEmpty()) return;
    if (item[0]->data(0, Qt::UserRole) == graphos::project) {

    } else if (item[0]->data(0, Qt::UserRole) == graphos::images) {
    } else if (item[0]->data(0, Qt::UserRole) == graphos::image ||
               item[0]->data(0, Qt::UserRole) == graphos::image_features ||
               item[0]->data(0, Qt::UserRole) == graphos::image_features_matches) {
        int size = item.size();
        if (size > 0) {
            if (size == 1) {
                emit select_image(item[0]->data(0, Qt::UserRole + 1).toULongLong());
            } else {
                std::vector<size_t> selected_images;
                for (int i = 0; i < size; i++) {
                    selected_images.push_back(item[i]->data(0, Qt::UserRole + 1).toULongLong());
                }
                emit select_images(selected_images);
            }
        }
    }
}

void MainWindowView::onItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item) {
        if (item->data(0, Qt::UserRole) == graphos::image ||
            item->data(0, Qt::UserRole) == graphos::image_features ||
            item->data(0, Qt::UserRole) == graphos::image_features_matches) {
            emit open_image(item->data(column, Qt::UserRole + 1).toULongLong());
        } else if (item->data(0, Qt::UserRole) == graphos::sparse_model) {
            emit open3DModel(item->toolTip(column), true);
        } else if (item->data(0, Qt::UserRole) == graphos::dense_model) {
            emit open3DModel(item->toolTip(column), false);
        } else if (item->data(0, Qt::UserRole) == graphos::mesh) {
            emit open3DModel(item->toolTip(column), false);
        } else if (item->data(0, Qt::UserRole) == graphos::dsm) {
            emit openDtm();
        } else if (item->data(0, Qt::UserRole) == graphos::ortho) {
            emit openOrtho(item->toolTip(column));
        }
    }
}

void MainWindowView::onTreeContextMenu(const QPoint &point)
{
    QPoint globalPos = mTreeWidgetProject->mapToGlobal(point);

    QTreeWidgetItem *item = nullptr;
    item = mTreeWidgetProject->itemAt(point);

    if (item == nullptr)
        return;

    if (item->data(0, Qt::UserRole) == graphos::project) {
        mMenuTreeProjectImages->exec(globalPos);
    } else if (item->data(0, Qt::UserRole) == graphos::images) {

    } else if (item->data(0, Qt::UserRole) == graphos::image ||
               item->data(0, Qt::UserRole) == graphos::image_features ||
               item->data(0, Qt::UserRole) == graphos::image_features_matches) {

        if (item->data(0, Qt::UserRole) == graphos::image) {
            mActionViewKeypoints->setEnabled(false);
            mActionViewMatches->setEnabled(false);
        } else if (item->data(0, Qt::UserRole) == graphos::image_features) {
            mActionViewKeypoints->setEnabled(true);
            mActionViewMatches->setEnabled(false);
        } else {
            mActionViewKeypoints->setEnabled(true);
            mActionViewMatches->setEnabled(true);
        }

        if (QAction *selectedItem = mMenuTreeProjectImage->exec(globalPos)) {
            if (selectedItem->text() == tr("Open Image")) {
                emit open_image(item->data(0, Qt::UserRole + 1).toULongLong());
            } else if (selectedItem->text() == tr("Delete Image")) {
                emit delete_images(std::vector<size_t>{item->data(0, Qt::UserRole + 1).toULongLong()});
            } else if (selectedItem->text() == tr("View Keypoints")) {
                emit openKeypointsViewer(item->data(0, Qt::UserRole + 1).toULongLong());
            } else if (selectedItem->text() == tr("View Matches")) {
                emit openMatchesViewer(item->data(0, Qt::UserRole + 1).toULongLong());
            }
        }
    } else if (item->data(0, Qt::UserRole) == graphos::sparse_model) {
        if (QAction *selectedItem = mMenuTreeProjectModel3D->exec(globalPos)) {
            if (selectedItem->text() == tr("Open Point Cloud")) {
                emit open3DModel(item->toolTip(0), true);
            }
        }
    } else if (item->data(0, Qt::UserRole) == graphos::dense_model) {
        if (QAction *selectedItem = mMenuTreeProjectModel3D->exec(globalPos)) {
            if (selectedItem->text() == tr("Open Point Cloud")) {
                emit open3DModel(item->toolTip(0), false);
            }
        }
    }
}

void MainWindowView::initUI()
{
    setWindowTitle(QString(""));

    mLayoutCentral = new QGridLayout(this->centralWidget());
    mLayoutCentral->setSpacing(6);
    mLayoutCentral->setContentsMargins(0, 0, 0, 0);

    this->initTabWidget();
    this->initThumbnailsTool();
    this->initConsole();

    this->initActions();
    this->initToolbars();
    this->initTreeWidget();
    this->initMenus();
    this->initProgressBar();

    // Configuración de mensajes
    auto &msg_h = tl::Message::instance();
    msg_h.addMessageHandler(mLogWidget);

    this->readSettings();

    this->retranslate();
    this->update();
}


void MainWindowView::initTabWidget()
{
    mTabWidget = new TabWidget(this->centralWidget());
    mLayoutCentral->addWidget(mTabWidget);
}

void MainWindowView::initThumbnailsTool()
{
    ui->dockWidgetThumb->toggleViewAction()->setIcon(QIcon::fromTheme("medium-icons"));

    QGridLayout *gridLayoutThumb = new QGridLayout(ui->dockWidgetThumbContents);
    gridLayoutThumb->setSpacing(6);
    gridLayoutThumb->setContentsMargins(11, 11, 11, 11);
    gridLayoutThumb->setContentsMargins(0, 0, 0, 0);
    mThumbnailsWidget = new ThumbnailsWidget(ui->dockWidgetThumbContents);
    gridLayoutThumb->addWidget(mThumbnailsWidget, 0, 0, 1, 1);
}

void MainWindowView::initConsole()
{
    ui->dockWidgetConsole->toggleViewAction()->setIcon(QIcon::fromTheme("console"));

    QGridLayout *gridLayoutConsole = new QGridLayout(ui->dockWidgetContentsConsole);
    gridLayoutConsole->setSpacing(6);
    gridLayoutConsole->setContentsMargins(11, 11, 11, 11);
    gridLayoutConsole->setContentsMargins(0, 0, 0, 0);
    mLogWidget = new LogWidget(ui->dockWidgetThumbContents);
    gridLayoutConsole->addWidget(mLogWidget, 0, 0, 1, 1);
}

void MainWindowView::initActions()
{
    /* File menu actions */

    mActionNewProject->setIcon(QIcon::fromTheme("new-project"));
    mActionOpenProject->setIcon(QIcon::fromTheme("open-project"));
    mActionSaveProject->setIcon(QIcon::fromTheme("save"));
    mActionSaveProjectAs->setIcon(QIcon::fromTheme("save-as"));
    mActionCloseProject->setIcon(QIcon::fromTheme("close-project"));
    mActionExit->setIcon(QIcon::fromTheme("exit"));

    /* View menu actions */

    mActionStartPage->setIcon(QIcon::fromTheme("start-page"));

    /* Help menu actions */

    mActionHelp->setIcon(QIcon::fromTheme("help"));

    /* Project tree context menu actions */

    mActionOpenImage->setIcon(QIcon::fromTheme("open-image"));
    mActionDeleteImage->setIcon(QIcon::fromTheme("remove-image"));
    mActionOpenModel3D->setIcon(QIcon::fromTheme("dense"));
    mActionViewKeypoints->setIcon(QIcon::fromTheme("features-viewer"));
    mActionViewMatches->setIcon(QIcon::fromTheme("matches-viewer"));

    /* Image viewer actions */

    mActionZoomIn->setIcon(QIcon::fromTheme("zoom-in"));
    mActionZoomOut->setIcon(QIcon::fromTheme("zoom-out"));
    mActionZoomExtend->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionZoom11->setIcon(QIcon::fromTheme("zoom-to-actual-size"));

    /* 3D viewer actions */

    mActionGlobalZoom->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionViewFront->setIcon(QIcon::fromTheme("front-view"));
    mActionViewTop->setIcon(QIcon::fromTheme("top-view"));
    mActionViewLeft->setIcon(QIcon::fromTheme("left-view"));
    mActionViewRight->setIcon(QIcon::fromTheme("right-view"));
    mActionViewBack->setIcon(QIcon::fromTheme("back-view"));
    mActionViewBottom->setIcon(QIcon::fromTheme("bottom-view"));
    mActionPointMeasuse->setIcon(QIcon::fromTheme("point_measuse"));
    mActionPointMeasuse->setCheckable(true);
    mActionDistanceMeasuse->setIcon(QIcon::fromTheme("distance"));
    mActionDistanceMeasuse->setCheckable(true);
    mActionAngleMeasure->setIcon(QIcon::fromTheme("angle_measure"));
    mActionAngleMeasure->setCheckable(true);
}

void MainWindowView::initToolbars()
{
    this->initToolbarFile();
    this->initToolbarWorkflow();
    this->initToolbarView();
    this->initToolbar3dModel();
    this->initToolbarTools();
}

void MainWindowView::initToolbarFile()
{
    mToolBarFile = new QToolBar(this);
    mToolBarFile->setObjectName("ToolBarFile");
    mToolBarFile->addAction(mActionNewProject);
    mToolBarFile->addAction(mActionOpenProject);
    mToolBarFile->addAction(mActionSaveProject);
    mToolBarFile->addAction(mActionSaveProjectAs);
    this->addToolBar(Qt::TopToolBarArea, mToolBarFile);
}

void MainWindowView::initToolbarWorkflow()
{
    mToolBarWorkflow = new QToolBar(this);
    mToolBarWorkflow->setObjectName("ToolBarWorkflow");
    this->addToolBar(Qt::TopToolBarArea, mToolBarWorkflow);
}

void MainWindowView::initToolbarView()
{
    mToolBarView = new QToolBar(this);
    mToolBarView->setObjectName("ToolBarView");
    mToolBarView->addAction(mActionZoomIn);
    mToolBarView->addAction(mActionZoomOut);
    mToolBarView->addAction(mActionZoomExtend);
    mToolBarView->addAction(mActionZoom11);
    this->addToolBar(Qt::TopToolBarArea, mToolBarView);
}

void MainWindowView::initToolbar3dModel()
{
    mToolBar3dModel = new QToolBar(this);
    mToolBar3dModel->setObjectName("ToolBar3dModel");
    mToolBar3dModel->addAction(mActionGlobalZoom);
    mToolBar3dModel->addSeparator();
    mToolBar3dModel->addAction(mActionViewFront);
    mToolBar3dModel->addAction(mActionViewBack);
    mToolBar3dModel->addAction(mActionViewTop);
    mToolBar3dModel->addAction(mActionViewBottom);
    mToolBar3dModel->addAction(mActionViewLeft);
    mToolBar3dModel->addAction(mActionViewRight);
    mToolBar3dModel->addSeparator();
    mToolBar3dModel->addAction(mActionPointMeasuse);
    mToolBar3dModel->addAction(mActionDistanceMeasuse);
    mToolBar3dModel->addAction(mActionAngleMeasure);
    this->addToolBar(Qt::TopToolBarArea, mToolBar3dModel);
}

void MainWindowView::initToolbarTools()
{
    mToolBarTools = new QToolBar(this);
    mToolBarTools->setObjectName("ToolBarTools");
    mToolBarTools->addAction(mActionCameras);
    this->addToolBar(Qt::TopToolBarArea, mToolBarTools);
}

void MainWindowView::initTreeWidget()
{
    ui->dockWidgetProject->toggleViewAction()->setIcon(QIcon::fromTheme("project-tree"));

    mTreeWidgetProject = new QTreeWidget(ui->dockWidgetContentsProject);
    mTreeWidgetProject->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidgetProject->header()->close();
    mTreeWidgetProject->setColumnCount(1);
    mTreeWidgetProject->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    mTreeWidgetProject->header()->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    mTreeWidgetProject->header()->setStretchLastSection(false);
    ui->gridLayout->addWidget(mTreeWidgetProject, 0, 0, 1, 1);

    /* Context menu*/

    mMenuTreeProjectImages = new QMenu(this);
    mMenuTreeProjectImage = new QMenu(this);
    mMenuTreeProjectImage->addAction(mActionOpenImage);
    mMenuTreeProjectImage->addAction(mActionDeleteImage);
    mMenuTreeProjectImage->addSeparator();
    mMenuTreeProjectImage->addAction(mActionViewKeypoints);
    mMenuTreeProjectImage->addSeparator();
    mMenuTreeProjectImage->addAction(mActionViewMatches);
    mMenuTreeProjectModel3D = new QMenu(this);
    mMenuTreeProjectModel3D->addAction(mActionOpenModel3D);
}

void MainWindowView::initMenus()
{
    this->initMenuFile();
    this->initMenuView();
    this->initMenuWorkflow();
    this->initMenuTools();
    this->initMenuPlugins();
    this->initMenuHelp();
}

void MainWindowView::initMenuFile()
{
    ui->menuFile->addAction(mActionNewProject);
    ui->menuFile->addAction(mActionOpenProject);
    mMenuRecentProjects = new QMenu(this);
    ui->menuFile->addMenu(mMenuRecentProjects);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(mActionSaveProject);
    ui->menuFile->addAction(mActionSaveProjectAs);
    ui->menuFile->addSeparator();
    mMenuImport = new QMenu(this);
    mMenuImport->setIcon(QIcon::fromTheme("import"));
    mMenuImport->addAction(mActionImportCameras);
    ui->menuFile->addMenu(mMenuImport);
    ui->menuFile->addSeparator();
    mMenuExport = new QMenu(this);
    mMenuExport->setIcon(QIcon::fromTheme("export"));
    mMenuExport->addAction(mActionExportCameras);
    //mMenuExport->addAction(mActionExportTiePoints);
    //mMenuExport->addAction(mActionExportMatches);
    //mMenuExport->addAction(mActionExportOrientations);
    //mMenuExport->addAction(mActionExportPointCloud);
    ui->menuFile->addMenu(mMenuExport);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(mActionCloseProject);
    ui->menuFile->addSeparator();
    ui->menuFile->addAction(mActionExit);
}

void MainWindowView::initMenuView()
{
    ui->menuView->addAction(mActionStartPage);
    ui->menuView->addSeparator();
    mMenuPanels = new QMenu(this);
    mMenuPanels->setIcon(QIcon::fromTheme("dock-panel"));
    mMenuPanels->addAction(ui->dockWidgetProject->toggleViewAction());
    mMenuPanels->addAction(ui->dockWidgetProperties->toggleViewAction());
    mMenuPanels->addAction(ui->dockWidgetConsole->toggleViewAction());
    mMenuPanels->addAction(ui->dockWidgetThumb->toggleViewAction());
    ui->menuView->addMenu(mMenuPanels);

    ui->menuView->addSeparator();

    mMenuToolBar = new QMenu(this);
    mMenuToolBar->setIcon(QIcon::fromTheme("toolbar"));
    mMenuToolBar->addAction(mToolBarFile->toggleViewAction());
    mMenuToolBar->addAction(mToolBarWorkflow->toggleViewAction());
    mMenuToolBar->addAction(mToolBarView->toggleViewAction());
    mMenuToolBar->addAction(mToolBar3dModel->toggleViewAction());
    mMenuToolBar->addAction(mToolBarTools->toggleViewAction());
    ui->menuView->addMenu(mMenuToolBar);
}

void MainWindowView::initMenuWorkflow()
{
}

void MainWindowView::initMenuTools()
{
    mActionCameras->setVisible(false);
    ui->menuTools->addAction(mActionCameras);
    ui->menuTools->addSeparator();
}

void MainWindowView::initMenuPlugins()
{

}

void MainWindowView::initMenuHelp()
{
    ui->menuHelp->addAction(mActionHelp);
}

void MainWindowView::initProgressBar()
{
    mProgressBar = new ProgressBarWidget(this);
    mProgressBar->setVisible(false);
    mProgressBar->setMaximumSize(220, 20);
    ui->statusBar->addPermanentWidget(mProgressBar);
}

void MainWindowView::initSignalAndSlots()
{
    /* File menu */

    connect(mActionExportTiePoints, &QAction::triggered, this, &MainWindowView::openExportFeatures);
    connect(mActionExportMatches, &QAction::triggered, this, &MainWindowView::openExportMatches);
    connect(mActionExportPointCloud, &QAction::triggered, this, &MainWindowView::openExportPointCloud);
    connect(mActionExit, &QAction::triggered, this, &MainWindowView::close);

    /* View menu */

    connect(mActionStartPage, &QAction::triggered, this, &MainWindowView::openStartPage);

    /* Help menu */

    connect(mActionHelp, SIGNAL(triggered(bool)), this, SIGNAL(openHelpDialog()));


    /* TreeWidgetProject */

    connect(mTreeWidgetProject, &QTreeWidget::itemSelectionChanged, this, &MainWindowView::onSelectionChanged);
    connect(mTreeWidgetProject, &QTreeWidget::itemDoubleClicked, this, &MainWindowView::onItemDoubleClicked);
    connect(mTreeWidgetProject, &QTreeWidget::customContextMenuRequested, this, &MainWindowView::onTreeContextMenu);


    /* Thumbnails */

    connect(mThumbnailsWidget, &ThumbnailsWidget::open_image, this, &MainWindowView::open_image);
    connect(mThumbnailsWidget, &ThumbnailsWidget::select_image, this, &MainWindowView::select_image);
    connect(mThumbnailsWidget, &ThumbnailsWidget::select_images, this, &MainWindowView::select_images);
    connect(mThumbnailsWidget, &ThumbnailsWidget::delete_images, this, &MainWindowView::delete_images);


    /* Zoom */

    connect(mActionZoomIn, &QAction::triggered, [&]() {
        if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
            graphic_viewer->zoomIn();
        }
    });

    connect(mActionZoomOut, &QAction::triggered, [&]() {
        if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
            graphic_viewer->zoomOut();
        }
    });

    connect(mActionZoomExtend, &QAction::triggered, [&]() {
        if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
            graphic_viewer->zoomExtend();
        }
    });

    connect(mActionZoom11, &QAction::triggered, [&]() {
        if (auto *graphic_viewer = dynamic_cast<GraphicViewer *>(mTabWidget->currentWidget())) {
            graphic_viewer->zoom11();
        }
    });

    connect(mActionGlobalZoom, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setGlobalZoom();
        }
    });

    connect(mActionViewFront, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setFrontView();
        }
    });

    connect(mActionViewBack, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setBackView();
        }
    });

    connect(mActionViewTop, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setTopView();
        }
    });

    connect(mActionViewBottom, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setBottomView();
        }
    });

    connect(mActionViewLeft, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setLeftView();
        }
    });

    connect(mActionViewRight, &QAction::triggered, [&]() {
        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            model3D->setRightView();
        }
    });

    connect(mActionPointMeasuse, &QAction::toggled, [&](bool checked) {

        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            const QSignalBlocker block_distance_measuse(mActionDistanceMeasuse);
            const QSignalBlocker block_angle_measuse(mActionAngleMeasure);
            mActionDistanceMeasuse->setChecked(false);
            mActionAngleMeasure->setChecked(false);

            if (checked)
                model3D->activatePicker(CCViewer3D::PickingMode::point_info);
            else
                model3D->deactivatePicker();
        }

    });

    connect(mActionDistanceMeasuse, &QAction::toggled, [&](bool checked) {

        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            const QSignalBlocker block_point_measuse(mActionPointMeasuse);
            const QSignalBlocker block_angle_measuse(mActionAngleMeasure);
            mActionPointMeasuse->setChecked(false);
            mActionAngleMeasure->setChecked(false);

            if (checked)
                model3D->activatePicker(CCViewer3D::PickingMode::distance);
            else
                model3D->deactivatePicker();
        }
    });

    connect(mActionAngleMeasure, &QAction::toggled, [&](bool checked) {

        if (auto *model3D = dynamic_cast<CCViewer3D *>(mTabWidget->currentWidget())) {
            const QSignalBlocker block_point_measuse(mActionPointMeasuse);
            const QSignalBlocker block_distance_measuse(mActionDistanceMeasuse);
            mActionPointMeasuse->setChecked(false);
            mActionDistanceMeasuse->setChecked(false);

            if (checked)
                model3D->activatePicker(CCViewer3D::PickingMode::angle);
            else
                model3D->deactivatePicker();
        }
    });

    /* TabWidget */

    connect(mTabWidget, &TabWidget::all_tabs_closed, this, &MainWindowView::all_tabs_closed);

    connect(mTabWidget, &TabWidget::imageActive, [&](bool active) {
        Application &app = Application::instance();
        AppStatus *app_status = app.status();
        app_status->activeFlag(AppStatus::Flag::tab_image_active, active);
    });

    connect(mTabWidget, &TabWidget::model3dActive, [&](bool active) {
        Application &app = Application::instance();
        AppStatus *app_status = app.status();
        app_status->activeFlag(AppStatus::Flag::tab_3d_viewer_active, active);
    });
}

QMenu *MainWindowView::findMenu(Menu menu)
{
    QMenu *_menu = nullptr;
    switch (menu) {
    case Menu::file:
        _menu = ui->menuFile;
        break;
    case Menu::file_import:
        _menu = mMenuImport;
        break;
    case Menu::file_export:
        _menu = mMenuExport;
        break;
    case Menu::view:
        _menu = ui->menuView;
        break;
    case Menu::workflow:
        _menu = ui->menuWorkflow;
        break;
    case Menu::tools:
        _menu = ui->menuTools;
        break;
    case Menu::plugins:
        _menu = ui->menuPlugins;
        break;
    case Menu::help:
        _menu = ui->menuHelp;
        break;
    }

    return _menu;
}

QToolBar *MainWindowView::findToolbar(Toolbar toolbar)
{
    QToolBar *_toolbar = nullptr;

    switch (toolbar) {
    case Toolbar::file:
        _toolbar = mToolBarFile;
        break;
    case Toolbar::view:
        _toolbar = mToolBarView;
        break;
    case Toolbar::workflow:
        _toolbar = mToolBarWorkflow;
        break;
    case Toolbar::model3d:
        _toolbar = mToolBar3dModel;
        break;
    case Toolbar::tools:
        _toolbar = mToolBarTools;
        break;
    default:
        break;
    }

    return _toolbar;
}

void MainWindowView::update()
{

    Application &app = Application::instance();
    AppStatus *app_status = app.status();
    bool project_exists = app_status->isEnabled(AppStatus::Flag::project_exists);
    bool project_modified = app_status->isEnabled(AppStatus::Flag::project_modified);
    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool image_active = app_status->isEnabled(AppStatus::Flag::tab_image_active);
    bool model_3d_active = app_status->isEnabled(AppStatus::Flag::tab_3d_viewer_active);

    mActionZoomIn->setEnabled(image_active);
    mActionZoomOut->setEnabled(image_active);
    mActionZoomExtend->setEnabled(image_active);
    mActionZoom11->setEnabled(image_active);

    mActionGlobalZoom->setEnabled(model_3d_active);
    mActionViewFront->setEnabled(model_3d_active);
    mActionViewTop->setEnabled(model_3d_active);
    mActionViewLeft->setEnabled(model_3d_active);
    mActionViewRight->setEnabled(model_3d_active);
    mActionViewBack->setEnabled(model_3d_active);
    mActionViewBottom->setEnabled(model_3d_active);
    mActionPointMeasuse->setEnabled(model_3d_active);
    mActionDistanceMeasuse->setEnabled(model_3d_active);
    mActionAngleMeasure->setEnabled(model_3d_active);

    mActionExit->setEnabled(!processing);
}

void MainWindowView::retranslate()
{
    mActionNewProject->setText(QApplication::translate("MainWindowView", "New Project", nullptr));
    mActionOpenProject->setText(QApplication::translate("MainWindowView", "Open Project", nullptr));
    mActionSaveProject->setText(QApplication::translate("MainWindowView", "Save Project", nullptr));
    mActionSaveProjectAs->setText(QApplication::translate("MainWindowView", "Save Project As...", nullptr));
    mActionImportCameras->setText(QApplication::translate("MainWindowView", "Import Cameras", nullptr));
    mActionExportCameras->setText(QApplication::translate("MainWindowView", "Export Cameras", nullptr));
    mActionExportTiePoints->setText(QApplication::translate("MainWindowView", "Export tie points", nullptr));
    mActionExportMatches->setText(QApplication::translate("MainWindowView", "Export Matches", nullptr));
    mActionExportPointCloud->setText(QApplication::translate("MainWindowView", "Export Point Cloud", nullptr));
    mActionCloseProject->setText(QApplication::translate("MainWindowView", "Close Project", nullptr));
    mActionExit->setText(QApplication::translate("MainWindowView", "Exit", nullptr));
    mActionStartPage->setText(QApplication::translate("MainWindowView", "Start Page", nullptr));
    mActionCameras->setText(QApplication::translate("MainWindowView", "Cameras", nullptr));
    mActionHelp->setText(QApplication::translate("MainWindowView", "Help", nullptr));
    mActionOpenImage->setText(QApplication::translate("MainWindowView", "Open Image", nullptr));
    mActionDeleteImage->setText(QApplication::translate("MainWindowView", "Delete Image", nullptr));
    mActionViewKeypoints->setText(QApplication::translate("MainWindowView", "View Keypoints", nullptr));
    mActionViewMatches->setText(QApplication::translate("MainWindowView", "View Matches", nullptr));
    mActionOpenModel3D->setText(QApplication::translate("MainWindowView", "Open Point Cloud", nullptr));
    mActionZoomIn->setText(QApplication::translate("MainWindowView", "Zoom In"));
    mActionZoomOut->setText(QApplication::translate("MainWindowView", "Zoom Out"));
    mActionZoomExtend->setText(QApplication::translate("MainWindowView", "Zoom Extend"));
    mActionZoom11->setText(QApplication::translate("MainWindowView", "Zoom 1:1"));
    mActionGlobalZoom->setText(QApplication::translate("MainWindowView", "Global Zoom", nullptr));
    mActionViewFront->setText(QApplication::translate("MainWindowView", "Front View", nullptr));
    mActionViewTop->setText(QApplication::translate("MainWindowView", "Top View", nullptr));
    mActionViewLeft->setText(QApplication::translate("MainWindowView", "Left View", nullptr));
    mActionViewRight->setText(QApplication::translate("MainWindowView", "Right View", nullptr));
    mActionViewBack->setText(QApplication::translate("MainWindowView", "Back View", nullptr));
    mActionViewBottom->setText(QApplication::translate("MainWindowView", "Bottom View", nullptr));
    mActionPointMeasuse->setText(QApplication::translate("MainWindowView", "Point Measure", nullptr));
    mActionDistanceMeasuse->setText(QApplication::translate("MainWindowView", "Distance Measure", nullptr));
    mActionAngleMeasure->setText(QApplication::translate("MainWindowView", "Angle Measure", nullptr));

    /* Toolbars */

    mToolBarFile->setWindowTitle(QCoreApplication::translate("MainWindowView", "File", nullptr));
    mToolBarWorkflow->setWindowTitle(QCoreApplication::translate("MainWindowView", "Workflow", nullptr));
    mToolBarView->setWindowTitle(QCoreApplication::translate("MainWindowView", "Image viewer tools", nullptr));
    mToolBar3dModel->setWindowTitle(QCoreApplication::translate("MainWindowView", "3D Viewer", nullptr));
    mToolBarTools->setWindowTitle(QCoreApplication::translate("MainWindowView", "Tools", nullptr));

    mMenuRecentProjects->setTitle(QApplication::translate("MainWindowView", "Recent Projects", nullptr));
    mMenuImport->setTitle(QApplication::translate("MainWindowView", "Import", nullptr));
    mMenuExport->setTitle(QApplication::translate("MainWindowView", "Export", nullptr));
    mMenuPanels->setTitle(QApplication::translate("MainWindowView", "Dockable panels", nullptr));
    mMenuToolBar->setTitle(QApplication::translate("MainWindowView", "Toolbars", nullptr));

#ifndef QT_NO_SHORTCUT
    mActionNewProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+N", nullptr));
    mActionOpenProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+O", nullptr));
    mActionSaveProject->setShortcut(QApplication::translate("MainWindowView", "Ctrl+S", nullptr));
    mActionSaveProjectAs->setShortcut(QApplication::translate("MainWindowView", "Ctrl+Shift+S", nullptr));
    mActionExit->setShortcut(QApplication::translate("MainWindowView", "Ctrl+F4", nullptr));
    mActionHelp->setShortcut(QApplication::translate("MainWindowView", "F1", nullptr));
#endif // QT_NO_SHORTCUT
}

void MainWindowView::closeEvent(QCloseEvent *event)
{
    emit exit();

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindowView::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

} // namespace graphos
