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
#include <tidop/core/progress.h>
#include <tidop/core/path.h>

#include "graphos/core/features/features.h"


namespace colmap
{
class Thread;
}

namespace graphos
{

class FeatureMatchingProperties
  : public FeatureMatching
{

public:

    FeatureMatchingProperties();
    FeatureMatchingProperties(const FeatureMatchingProperties &featureMatching);
    ~FeatureMatchingProperties() override = default;

// FeatureMatching interface

public:

    bool crossCheck() const override;
    void enableCrossCheck(bool enable) override;
    double ratio() const override;
    void setRatio(double ratio) override;
    double distance() const override;
    void setDistance(double distance) override;
    double maxError() const override;
    void setMaxError(double error) override;
    double confidence() const override;
    void setConfidence(double error) override;

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

    FeatureMatchingTask(const tl::Path &database,
                        bool cuda,
                        const std::shared_ptr<FeatureMatching> &featureMatching);
    ~FeatureMatchingTask() override;

public:

    std::shared_ptr<FeatureMatching> featureMatching() const;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

    tl::Path database() const;
    void setDatabase(const tl::Path &database);

    bool useGPU() const;
    void setUseGPU(bool useGPU);

// tl::TaskBase interface

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar) override;

private:

    colmap::Thread *mFeatureMatcher;
    tl::Path mDatabase;
    bool bUseCuda;
    std::shared_ptr<FeatureMatching> mFeatureMatching;

};



class SpatialMatchingTask
  : public QObject,
    public tl::TaskBase
{

    Q_OBJECT

public:

    SpatialMatchingTask(const tl::Path &database,
                        bool cuda,
                        const std::shared_ptr<FeatureMatching> &featureMatching);
    ~SpatialMatchingTask() override;

public:

    std::shared_ptr<FeatureMatching> featureMatching() const;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching);

    tl::Path database() const;
    void setDatabase(const tl::Path &database);

    bool useGPU() const;
    void setUseGPU(bool useGPU);

// tl::TaskBase interface

public:

    void stop() override;

protected:

    void execute(tl::Progress *progressBar) override;

private:

    colmap::Thread *mFeatureMatcher;
    tl::Path mDatabase;
    bool bUseCuda;
    std::shared_ptr<FeatureMatching> mFeatureMatching;

};


} // namespace graphos

#endif // GRAPHOS_CORE_FEATURE_MATCHING_H
