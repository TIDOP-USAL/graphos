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

#ifndef GRAPHOS_SCALE_PRESENTER_H
#define GRAPHOS_SCALE_PRESENTER_H

#include "graphos/components/scale/ScalePresenter.h"

namespace graphos
{

class ScaleView;
class ScaleModel;

class ScalePresenterImp
  : public ScalePresenter
{

    Q_OBJECT

public:

    ScalePresenterImp(ScaleView *view,
                      ScaleModel *model);
    ~ScalePresenterImp() override;

public slots:

    void measure(bool active);

private slots:

    void pointClicked(const QVector3D &point);

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

// TaskPresenter interface

protected:

    auto createTask() -> std::unique_ptr<tl::Task> override;

protected slots:

    void onError(tl::TaskErrorEvent *event) override;
    void onFinished(tl::TaskFinalizedEvent *event) override;

public slots:

    void cancel() override;

private:

    ScaleView *mView;
    ScaleModel *mModel;
    std::vector<QVector3D> mPoints;
};

} // namespace graphos

#endif // GRAPHOS_SCALE_PRESENTER_H
