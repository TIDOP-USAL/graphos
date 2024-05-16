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


#include "FloorLevelComponent.h"

#include "graphos/core/project.h"
#include "graphos/core/AppStatus.h"
#include "graphos/widgets/Viewer3d.h"

TL_DISABLE_WARNINGS
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>
#include <cc2DLabel.h>
TL_DEFAULT_WARNINGS

#include <QAction>
#include <QString>
#include <QMessageBox>
#include <QMainWindow>

namespace graphos
{

FloorLevelComponent::FloorLevelComponent()
  : Component(),
    mLevelMarkersCloud(nullptr)
{
    init();
}

FloorLevelComponent::~FloorLevelComponent()
{
    if (mAction) {
        delete mAction;
        mAction = nullptr;
    }

    if (mLevelMarkersCloud) {
        delete mLevelMarkersCloud;
        mLevelMarkersCloud = nullptr;
    }
}

void FloorLevelComponent::open()
{
    if (auto viewer_3d = dynamic_cast<Application *>(qApp)->viewer3D()) {

        if (auto ccviewer = dynamic_cast<CCViewer3D *>(viewer_3d)) {
            const QSignalBlocker blocker2(ccviewer);
            dynamic_cast<Application *>(qApp)->mainWindow()->activateWindow();

            connect(ccviewer, SIGNAL(mouseClicked(QVector3D)), this, SLOT(pointClicked(QVector3D)));
            ccviewer->activatePicker(CCViewer3D::PickingMode::level_points);
            mPoints.reserve(3);
            dynamic_cast<ccGLWindow *>(viewer_3d)->displayNewMessage("Pick three points on the floor plane (click the Level button or press Escape to cancel)", ccGLWindow::LOWER_LEFT_MESSAGE, true, 24 * 3600);
        }

    } else {

        QMessageBox msgBox(QMessageBox::Warning,
                           "A 3D model is required for the measurement",
                           "Do you want to open the 3D model??",
                           QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            emit open_3d_model();
            open();
        }

    }
}

void FloorLevelComponent::pointClicked(const QVector3D &point)
{
    mPoints.push_back(point);

    auto viewer_3d = dynamic_cast<Application *>(qApp)->viewer3D();

    if (mPoints.size() == 1) {

        if (mLevelMarkersCloud) {
			dynamic_cast<ccGLWindow *>(viewer_3d)->removeFromOwnDB(mLevelMarkersCloud);
			delete mLevelMarkersCloud;
			mLevelMarkersCloud = nullptr;
		}

        mLevelMarkersCloud = new ccPointCloud("Level points");
        mLevelMarkersCloud->reserve(3);
		dynamic_cast<ccGLWindow *>(viewer_3d)->addToOwnDB(mLevelMarkersCloud);
    } 
            
    mLevelMarkersCloud->addPoint(CCVector3(point.x(), point.y(), point.z()));
    unsigned marker_count = mLevelMarkersCloud->size();
    cc2DLabel *label = new cc2DLabel();
    label->addPickedPoint(mLevelMarkersCloud, marker_count - 1);
    label->setName(QString("P#%1").arg(marker_count));
    label->setDisplayedIn2D(false);
    label->setDisplay(dynamic_cast<ccGLWindow *>(viewer_3d));
    label->setVisible(true);
    mLevelMarkersCloud->addChild(label);
    dynamic_cast<ccGLWindow *>(viewer_3d)->redraw();

    if (mPoints.size() == 3) {

        auto ccviewer = dynamic_cast<CCViewer3D *>(viewer_3d);

        auto &A = mPoints.at(0);
        auto &B = mPoints.at(1);
        auto &C = mPoints.at(2);

        auto X = B - A;
        auto Y = C - A;
        auto Z = QVector3D::crossProduct(X, Y);
        //we choose 'Z' so that it points 'upward' relatively to the camera (assuming the user will be looking from the top)
        CCVector3d viewDir = ccviewer->getViewportParameters().getViewDir();
        if (QVector3D::dotProduct(Z, QVector3D(static_cast<float>(viewDir.x), static_cast<float>(viewDir.y), static_cast<float>(viewDir.z))) > 0) {
            Z = -Z;
        }
        Y = QVector3D::crossProduct(Z, X);
        X.normalize();
        Y.normalize();
        Z.normalize();

        ccGLMatrix trans;
        float *mat = trans.data();
        mat[0] = X.x(); mat[4] = X.y(); mat[8] = X.z(); mat[12] = 0;
        mat[1] = Y.x(); mat[5] = Y.y(); mat[9] = Y.z(); mat[13] = 0;
        mat[2] = Z.x(); mat[6] = Z.y(); mat[10] = Z.z(); mat[14] = 0;
        mat[3] = 0; mat[7] = 0; mat[11] = 0; mat[15] = 1;

        CCVector3d T(-A.x(), -A.y(), -A.z());
        trans.apply(T);
        T += CCVector3d(A.x(), A.y(), A.z());
        trans.setTranslation(T);

        ccHObject *root = ccviewer->object();
        ccHObject::Container clouds;
        root->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        if (clouds.size() == 1) {
            ccHObject *model = clouds.at(0);

            bool locked_vertices;
            ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(model, &locked_vertices);

            cloud->applyRigidTransformation(trans);
            model->prepareDisplayForRefresh_recursive();
        }

        auto transform = tl::Matrix<double, 4, 4>::identity();
        transform(0, 0) = mat[0];
        transform(1, 0) = mat[1];
        transform(2, 0) = mat[2];
        
        transform(0, 1) = mat[4];
        transform(1, 1) = mat[5];
        transform(2, 1) = mat[6];
        
        transform(0, 2) = mat[8];
        transform(1, 2) = mat[9];
        transform(2, 2) = mat[10];

        transform(0, 3) = mat[3];
        transform(1, 3) = mat[7];
        transform(2, 3) = mat[11];

        auto &_transform = dynamic_cast<Application *>(qApp)->project()->transform();
        _transform = _transform * transform;
        //dynamic_cast<Application *>(qApp)->project()->setTransform(transform);

        if (mLevelMarkersCloud) {
            delete mLevelMarkersCloud;
            mLevelMarkersCloud = nullptr;
        }

        //clear message
        dynamic_cast<ccGLWindow *>(viewer_3d)->displayNewMessage(QString(), ccGLWindow::LOWER_LEFT_MESSAGE, false);
        dynamic_cast<ccGLWindow *>(viewer_3d)->setView(CC_TOP_VIEW);

        mPoints.resize(0);

        disconnect(ccviewer, SIGNAL(mouseClicked(QVector3D)), this, SLOT(pointClicked(QVector3D)));
        ccviewer->deactivatePicker();

        auto app = dynamic_cast<Application *>(qApp);
        AppStatus *app_status = app->status();

        app_status->activeFlag(AppStatus::Flag::project_modified, true);
    }
}

void FloorLevelComponent::init()
{
    mAction = new QAction();
    setName(tr("Floor Level"));
    setMenu("tools");
    connect(mAction, &QAction::triggered,
            this, &FloorLevelComponent::open);

    if (auto app = dynamic_cast<Application *>(qApp)) {
        connect(app->status(), &AppStatus::update,
                this, &FloorLevelComponent::update);
    }
}

void FloorLevelComponent::update()
{
    auto app = dynamic_cast<Application *>(qApp);
    TL_ASSERT(app != nullptr, "Application is null");
    AppStatus *app_status = app->status();
    TL_ASSERT(app_status != nullptr, "AppStatus is null");

    bool processing = app_status->isEnabled(AppStatus::Flag::processing);
    bool dense_model = app_status->isEnabled(AppStatus::Flag::dense_model);
    action()->setEnabled(!processing && dense_model);
}

auto FloorLevelComponent::name() const -> QString
{
    return mName;
}

auto FloorLevelComponent::action() const -> QAction*
{
    return mAction;
}

auto FloorLevelComponent::menu() const -> QString
{
    return mMenu;
}

auto FloorLevelComponent::toolbar() const -> QString
{
    return mToolbar;
}

auto FloorLevelComponent::widget() const -> QWidget*
{
    return nullptr;
}

auto FloorLevelComponent::command() -> std::shared_ptr<Command>
{
    return {};
}

void FloorLevelComponent::setName(const QString &name)
{
    mName = name;
    mAction->setText(name);
}

void FloorLevelComponent::setMenu(const QString &menu)
{
    mMenu = menu;
}

void FloorLevelComponent::setToolbar(const QString &toolbar)
{
    mToolbar = toolbar;
}

void FloorLevelComponent::setIcon(const QIcon &icon)
{
    action()->setIcon(icon);
}

void FloorLevelComponent::freeMemory()
{
    mCommand.reset();
}

} // namespace graphos
