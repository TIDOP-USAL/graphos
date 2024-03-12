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

#ifndef GRAPHOS_CORE_FEATURE_MATCHING_H
#define GRAPHOS_CORE_FEATURE_MATCHING_H

#include "graphos/graphos_global.h"

#include <QObject>

#include <tidop/core/task.h>
#include <tidop/core/path.h>

#include "graphos/core/features/features.h"

namespace graphos
{

class FeatureMatching
{

public:

    FeatureMatching();
    FeatureMatching(const FeatureMatching &featureMatching);
    virtual ~FeatureMatching() = default;

public:

    virtual auto crossCheck() const -> bool;
    virtual void enableCrossCheck(bool enable);
    virtual auto ratio() const -> double;
    virtual void setRatio(double ratio);
    virtual auto distance() const -> double;
    virtual void setDistance(double distance);
    virtual auto maxError() const -> double;
    virtual void setMaxError(double error);
    virtual auto confidence() const -> double;
    virtual void setConfidence(double error);

private:

    bool mCrossCheck;
    double mRatio;
    double mDistance;
    double mMaxError;
    double mConfidence;

};




class FeatureMatchingTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    FeatureMatchingTask(tl::Path database,
                        bool cuda,
                        const std::shared_ptr<FeatureMatching> &featureMatching);
    ~FeatureMatchingTask() override;

public:

    auto featureMatching() const -> std::shared_ptr<FeatureMatching>;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

    auto database() const -> tl::Path;
    void setDatabase(const tl::Path &database);

    auto useGPU() const -> bool;
    void setUseGPU(bool useGPU);

    auto report() const->FeatureMatchingReport;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mDatabase;
    bool bUseCuda;
    std::shared_ptr<FeatureMatching> mFeatureMatching;
    FeatureMatchingReport mReport;

};



class SpatialMatchingTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    SpatialMatchingTask(tl::Path database,
                        bool cuda,
                        const std::shared_ptr<FeatureMatching> &featureMatching);
    ~SpatialMatchingTask() override;

public:

    auto featureMatching() const -> std::shared_ptr<FeatureMatching>;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

    auto database() const -> tl::Path;
    void setDatabase(const tl::Path &database);

    auto useGPU() const -> bool;
    void setUseGPU(bool useGPU);

    auto report() const -> FeatureMatchingReport;

// tl::TaskBase interface

protected:

    void execute(tl::Progress *progressBar) override;

private:

    tl::Path mDatabase;
    bool bUseCuda;
    std::shared_ptr<FeatureMatching> mFeatureMatching;
    FeatureMatchingReport mReport;

};


} // namespace graphos

#endif // GRAPHOS_CORE_FEATURE_MATCHING_H
