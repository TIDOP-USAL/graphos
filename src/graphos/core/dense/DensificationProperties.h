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

#include "graphos/graphos_global.h"

#include <string>

#include <tidop/core/base/Property.h>

namespace graphos
{

class Camera;
class CameraPose;

class DensificationProperties
{

//public:
//
//    enum class Method
//    {
//        cmvs_pmvs,
//        smvs,
//        mvs
//    };

protected:

    tl::Properties mProperties;

public:

    DensificationProperties(std::string method)
      : mProperties(std::move(method)) {}
    virtual ~DensificationProperties() = default;

    /*!
     * \brief Reset the Densification properties.
     */
    virtual void clear() = 0;

    [[nodiscard]]
    auto name() const -> std::string;

    auto begin() const
    {
        return mProperties.begin();
    }

    auto end() const
    {
        return mProperties.end();
    }

    void setProperty(const std::string &key, const std::string &value);

    template<typename T>
    void setProperty(const std::string &key, T value)
    {
        mProperties.setProperty(key, value);
    }

    ///*!
    // * \brief Densification method
    // * \see Method
    // */
    //auto method() const -> Method { return mDensificationMethod.flags(); }

//protected:
//
//    tl::EnumFlags<Method> mDensificationMethod;

};
//ALLOW_BITWISE_FLAG_OPERATIONS(DensificationProperties::Method)


class DensificationPropertiesFactory
{

public:

    static auto create(const std::string &type) -> std::shared_ptr<DensificationProperties>;
};



//class Densifier
//{
//
//public:
//
//    Densifier() = default;
//    virtual ~Densifier() = default;
//
//    virtual void enableCuda(bool enable) = 0;
//    virtual auto isCudaEnabled() const -> bool = 0;
//    virtual auto denseModel() const -> tl::Path = 0;
//    virtual auto report() const -> DenseReport = 0;
//};
//
//
//
//class DensifierBase
//  : public Densifier,
//    public tl::Task
//{
//
//public:
//
//    DensifierBase(const std::unordered_map<size_t, Image> &images,
//                  const std::map<int, Camera> &cameras,
//                  const std::unordered_map<size_t, CameraPose> &poses,
//                  const std::vector<GroundPoint> &groundPoints,
//                  tl::Path outputPath);
//    ~DensifierBase() override;
//
//    void setUndistortImagesFormat(UndistortImages::Format format);
//
//protected:
//
//    void undistort(const QString &dir) const;
//    auto outputPath() const -> tl::Path;
//    auto images() const -> const std::unordered_map<size_t, Image>&;
//    auto cameras() const -> const std::map<int, Camera>&;
//    auto poses() const -> const std::unordered_map<size_t, CameraPose>&;
//    auto groundPoints() const -> const std::vector<GroundPoint>&;
//    void setDenseModel(const tl::Path &denseModel);
//    void autoSegmentation() const;
//    virtual void copyUndistortedImages() const = 0;
//
//// Densifier
//
//public:
//
//    void enableCuda(bool enable) override;
//    auto isCudaEnabled() const -> bool override;
//    auto denseModel() const -> tl::Path override;
//    auto report() const -> DenseReport override;
//
//private:
//
//    std::unordered_map<size_t, Image> mImages;
//    std::map<int, Camera> mCameras;
//    std::unordered_map<size_t, CameraPose> mPoses;
//    std::vector<GroundPoint> mGroundPoints;
//    tl::Path mOutputPath;
//    bool mCuda;
//    tl::Path mDenseModel;
//    UndistortImages::Format mFormat;
//
//protected:
//
//    DenseReport mReport;
//};



} // namespace graphos
