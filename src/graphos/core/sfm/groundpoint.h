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

#ifndef GRAPHOS_CORE_GROUND_CONTROL_POINT_H
#define GRAPHOS_CORE_GROUND_CONTROL_POINT_H

#include "graphos/graphos_global.h"

#include <unordered_map>
#include <functional>

#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/graphic/color.h>

#include "graphos/core/sfm/track.h"

namespace graphos
{

/* Ground Control Points */

class GroundControlPoint
    : public tl::Point3<double>
{

public:

    GroundControlPoint();
    GroundControlPoint(const tl::Point3<double> &point3d);
    ~GroundControlPoint() override;

    std::string name() const;
    void setName(const std::string &name);
    void setPoint(const tl::Point3<double> &point);
    void addPointToTrack(size_t imageId, const tl::Point<double> &point);
    void setTrack(const GCPTrack &track);
    void removeTrackPoint(size_t imageId);
    const GCPTrack &track() const;

private:

    std::string mName;
    GCPTrack mTrack;

};


/* Ground Control Points Reader */

class GCPsReader
{

public:

    GCPsReader();
    virtual ~GCPsReader() = default;

    virtual void read(const tl::Path &path) = 0;
    virtual std::string format() const = 0;

    std::vector<GroundControlPoint> gcps() const;
    std::string epsgCode() const;

protected:

    void addGroundControlPoint(const GroundControlPoint &gcp);
    void setEPSGCode(const std::string &epsgCode);

private:

    std::vector<GroundControlPoint> mGCPs;
    std::string mEpsgCode;
};


class GCPsReaderFactory
{

private:

    GCPsReaderFactory() = default;

public:

    static std::unique_ptr<GCPsReader> create(const std::string &format);

};


/* Ground Control Points Writer */

class GCPsWriter
{

public:

    GCPsWriter();
    virtual ~GCPsWriter() = default;

    virtual void write(const tl::Path &path) = 0;
    virtual std::string format() const = 0;

    void setGCPs(const std::vector<GroundControlPoint> &GCPs);
    void setEPSGCode(const std::string &epsgCode);

protected:

    std::vector<GroundControlPoint> gcps() const;
    std::string epsgCode() const;

private:

    std::vector<GroundControlPoint> mGCPs;
    std::string mEpsgCode;
};


class GCPsWriterFactory
{

private:

    GCPsWriterFactory() = default;

public:

    static std::unique_ptr<GCPsWriter> create(const std::string &format);

};



/* Ground Points */

class GroundPoint
    : public tl::Point3<double>
{

public:

    GroundPoint();
    GroundPoint(const tl::Point3<double> &point3d);
    ~GroundPoint() override;

    void setPoint(const tl::Point3<double> &point);
    tl::Color color() const;
    void setColor(const tl::Color &color);
    void addPairToTrack(size_t imageId,
                        size_t pointId);
    void setTrack(const Track &track);
    void removeTrackPair(size_t imageId);
    const Track &track() const;

private:

    tl::Color mColor;
    Track mTrack;

};


/* Ground Points Reader */

class GroundPointsReader
{

public:

    GroundPointsReader();
    virtual ~GroundPointsReader() = default;

    virtual void read(const tl::Path &path) = 0;
    virtual std::string format() const = 0;

    std::vector<GroundPoint> points() const;
    std::string epsgCode() const;

protected:

    void addGroundPoint(const GroundPoint &groundPoint);
    void setGroundPoints(const std::vector<GroundPoint> &groundPoint);
    void setEPSGCode(const std::string &epsgCode);

private:

    std::vector<GroundPoint> mGroundPoints;
    std::string mEpsgCode;
};


class GroundPointsReaderFactory
{

private:

    GroundPointsReaderFactory() = default;

public:

    static std::unique_ptr<GroundPointsReader> create(const std::string &format);

};


/* Ground Points Writer */

class GroundPointsWriter
{

public:

    GroundPointsWriter();
    virtual ~GroundPointsWriter() = default;

    virtual void write(const tl::Path &path) = 0;
    virtual std::string format() const = 0;

    void setGroundPoints(const std::vector<GroundPoint> &groundPoint);
    void setEPSGCode(const std::string &epsgCode);

protected:

    std::vector<GroundPoint> groundPoints() const;
    std::string epsgCode() const;

private:

    std::vector<GroundPoint> mGroundPoints;
    std::string mEpsgCode;
};


class GroundPointsWriterFactory
{

private:

    GroundPointsWriterFactory() = default;

public:

    static std::unique_ptr<GroundPointsWriter> create(const std::string &format);

};



} // namespace graphos

#endif // GRAPHOS_CORE_GROUND_CONTROL_POINT_H
