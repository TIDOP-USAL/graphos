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

#ifndef GRAPHOS_CORE_FEATURES_IO_H
#define GRAPHOS_CORE_FEATURES_IO_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

///TODO: Quitar la cabecera de aqui
#include <colmap/feature/sift.h>

namespace graphos
{

/*!
 * \brief FeaturesWriter class allows writing different formats of features files
 */
class FeaturesWriter
{

public:

    FeaturesWriter(const QString &fileName);
    virtual ~FeaturesWriter() = default;

    virtual bool write() = 0;

    void setKeyPoints(const colmap::FeatureKeypoints &keyPoints);
    void setDescriptors(const colmap::FeatureDescriptors &descriptors);

protected:

    QString mFileName;
    colmap::FeatureKeypoints mKeyPoints;
    colmap::FeatureDescriptors mDescriptors;
};


/*----------------------------------------------------------------*/


/*!
 * \brief The FeaturesWriter class allows reading different formats of features files
 */
class FeaturesReader
{

public:

    FeaturesReader(const QString &fileName);
    virtual ~FeaturesReader() = default;

    virtual bool read() = 0;

    auto keyPoints() const -> colmap::FeatureKeypoints;
    auto descriptors() const -> colmap::FeatureDescriptors;

protected:

    QString mFileName;
    colmap::FeatureKeypoints mKeyPoints;
    colmap::FeatureDescriptors mDescriptors;
};



/*----------------------------------------------------------------*/



/*!
 * \brief Factory class to create different reading formats
 */
class FeaturesReaderFactory
{

private:

    FeaturesReaderFactory() {}

public:

    static auto createReader(const QString& fileName) -> std::unique_ptr<FeaturesReader>;
};

/*!
 * \brief Factory class to create different writing formats
 */
class FeaturesWriterFactory
{
public:

private:

    FeaturesWriterFactory() {}

public:

    static auto createWriter(const QString& fileName) -> std::unique_ptr<FeaturesWriter>;
};



/*----------------------------------------------------------------*/



class FeaturesIOHandler
{

public:

    FeaturesIOHandler();
    virtual ~FeaturesIOHandler() = default;

    auto read(const QString& file) -> bool;
    auto write(const QString& file) -> bool;

protected:

    std::unique_ptr<FeaturesReader> mReader;
    std::unique_ptr<FeaturesWriter> mWriter;
};


} // namespace graphos


#endif // GRAPHOS_CORE_FEATIO_H
