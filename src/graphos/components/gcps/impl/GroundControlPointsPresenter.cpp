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

#include "GroundControlPointsPresenter.h"

#include "graphos/components/gcps/impl/GroundControlPointsModel.h"
#include "graphos/components/gcps/impl/GroundControlPointsView.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/image.h"

#include <tidop/core/defs.h>
#include <tidop/core/msg/message.h>

#include <QFileInfo>
#include <QAbstractItemModel>
#include <QStandardItemModel>

namespace graphos
{

GroundControlPointsPresenterImp::GroundControlPointsPresenterImp(GroundControlPointsView *view,
                                                                 GroundControlPointsModel *model)
  : mView(view),
    mModel(model)
{
    GroundControlPointsPresenterImp::init();
    GroundControlPointsPresenterImp::initSignalAndSlots();
}

GroundControlPointsPresenterImp::~GroundControlPointsPresenterImp()
{
}

void GroundControlPointsPresenterImp::open()
{
    mView->clear();

    mModel->loadGroundControlPoints();
    mView->setCrs(mModel->crs());
    mView->showMaximized();

    bool active_image = false;
    std::vector<std::pair<size_t, QString>> images;
    for (const auto &image : mModel->images()) {
        images.emplace_back(image.first, image.second.path());
    }

    if (!images.empty()) {
        mView->setImageList(images);
        setImageActive(images[0].first);
    }

}

void GroundControlPointsPresenterImp::init()
{
    mView->setItemModelGroundControlPoints(mModel->itemModelGroundControlPoints());
    mView->setItemModelImagePoints(mModel->itemModelImagePoints());
}

void GroundControlPointsPresenterImp::initSignalAndSlots()
{
    connect(mView, &GroundControlPointsView::image_changed, this, &GroundControlPointsPresenterImp::setImageActive);
    connect(mView, &GroundControlPointsView::addGroundControlPoint, mModel, &GroundControlPointsModel::addGroundControlPoint);
    connect(mView, &GroundControlPointsView::removeGroundControlPoint, mModel, &GroundControlPointsModel::removeGroundControlPoint);
    connect(mView, &GroundControlPointsView::add_image_point, mModel, &GroundControlPointsModel::addImagePoint);
    connect(mView, &GroundControlPointsView::remove_image_point, mModel, &GroundControlPointsModel::removeImagePoint);
    connect(mView, &GroundControlPointsView::crsChange, mModel, &GroundControlPointsModel::setCrs);

    connect(mView, &GroundControlPointsView::new_gcps, 
        [&]() {
            mView->clear();
            mModel->clear();
        });

    connect(mView, &GroundControlPointsView::save, mModel, &GroundControlPointsModel::save);
    connect(mView, &GroundControlPointsView::import_gcp, 
        [&](const QString &file, const QString &format) {
            try {
                mModel->importGroundControlPoints(file, format);
                mView->setCrs(mModel->crs());
            } catch (std::exception &e){
                tl::printException(e);
            }
        });
    connect(mView, &GroundControlPointsView::export_gcp, mModel, &GroundControlPointsModel::exportGroundControlPoints);

    connect(mView, &DialogView::help, [&]() {
        emit help("cameras.html");
    });

}

void GroundControlPointsPresenterImp::setImageActive(size_t imageId)
{
    auto image = mModel->image(imageId);
    mView->setCurrentImage(image.path());
    std::list<std::pair<QString, QPointF>> points = mModel->points(imageId);
    mView->setPoints(points);
}

} // namespace graphos
