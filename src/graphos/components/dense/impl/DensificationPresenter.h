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

#ifndef GRAPHOS_DENSE_PRESENTER_H
#define GRAPHOS_DENSE_PRESENTER_H

#include "graphos/components/dense/DensificationPresenter.h"

namespace graphos
{

class DensificationView;
class DensificationModel;

class DensificationPresenterImp
  : public DensificationPresenter
{
    Q_OBJECT

public:

    DensificationPresenterImp(DensificationView *view,
                              DensificationModel *model);
    ~DensificationPresenterImp() override;

private:

    void configureCmvsPmvsProperties() const;
    void configureSmvsProperties() const;
    void configureMvsProperties() const;

private slots:

    void onDensificationChanged(const QString &densification);

// DensificationPresenter interface

public:

    void setCmvsPmvsWidget(std::shared_ptr<CmvsPmvsWidget> cmvsPmvs) override;
    void setSmvsWidget(std::shared_ptr<SmvsWidget> smvs) override;
    void setMvsWidget(std::shared_ptr<MvsWidget> mvs) override;

public slots:

    void setCurrentDensifier(const QString &densifier) override;

// TaskPresenter interface

protected:

    void onError(tl::TaskErrorEvent *event) override;
    void onFinished(tl::TaskFinalizedEvent *event) override;
    auto createTask() -> std::unique_ptr<tl::Task> override;

public slots:

    void cancel() override;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

private:

    DensificationView *mView;
    DensificationModel *mModel;
    std::shared_ptr<CmvsPmvsWidget> mCmvsPmvs;
    std::shared_ptr<SmvsWidget> mSmvs;
    std::shared_ptr<MvsWidget> mMVS;
};

} // End namespace graphos


#endif // GRAPHOS_DENSE_PRESENTER_H
