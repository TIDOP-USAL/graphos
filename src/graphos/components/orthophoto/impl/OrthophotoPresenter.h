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

#pragma once

#include "graphos/components/orthophoto/OrthophotoPresenter.h"

#include <unordered_map>

#include <tidop/core/base/Path.h>

namespace graphos
{

class OrthophotoView;
class OrthophotoModel;
class Camera;
class Image;
class CameraRepository;
class ImageRepository;

class OrthophotoPresenterImp
  : public OrthophotoPresenter
{

    Q_OBJECT

public:

    OrthophotoPresenterImp(OrthophotoView *view,
                           OrthophotoModel *model);
    ~OrthophotoPresenterImp() override;

    auto undistortedCameras(const CameraRepository &cameras) const -> std::map<int, Camera>;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

// ProcessPresenter interface

protected:

    void onError(tl::TaskErrorEvent &event) override;
    void onFinished(tl::TaskFinalizedEvent &event) override;
    auto createTask() -> std::unique_ptr<tl::Task> override;

    auto undistortedImages(const ImageRepository &images,
                           tl::Path &undistort_path) const -> std::unordered_map<size_t, Image>;

public slots:

    void cancel() override;

private:

    OrthophotoView *mView;
    OrthophotoModel *mModel;

};

} // namespace graphos
