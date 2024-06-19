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

#include "graphos/core/features/featio.h"

#include <tidop/core/exception.h>

#include <colmap/base/database.h>

#include <QFileInfo>

#include <stdexcept>

namespace graphos
{

FeaturesWriter::FeaturesWriter(const QString &fileName)
    : mFileName(fileName)
{

}

void FeaturesWriter::setKeyPoints(const colmap::FeatureKeypoints &keyPoints)
{
    mKeyPoints = keyPoints;
}

void FeaturesWriter::setDescriptors(const colmap::FeatureDescriptors &descriptors)
{
    mDescriptors = descriptors;
}


/* ---------------------------------------------------------------------------------- */



FeaturesReader::FeaturesReader(const QString &fileName)
    : mFileName(fileName)
{

}

colmap::FeatureKeypoints FeaturesReader::keyPoints() const
{
    return mKeyPoints;
}

colmap::FeatureDescriptors FeaturesReader::descriptors() const
{
    return mDescriptors;
}



/* ---------------------------------------------------------------------------------- */



class FeaturesReaderColmap
  : public FeaturesReader
{

public:

    explicit FeaturesReaderColmap(const QString &fileName);
    ~FeaturesReaderColmap() override = default;

// FeaturesReader interface

public:

    bool read() override;

private:

    void open();
    auto isOpen() const -> bool;
    void readDB() const;
    void close();

private:

    std::unique_ptr<colmap::Database> mDatabase;
    colmap::Image mImageId;
    bool bOpen = false;
};

FeaturesReaderColmap::FeaturesReaderColmap(const QString &fileName)
  : FeaturesReader(fileName),
    mDatabase(new colmap::Database)
{

}

auto FeaturesReaderColmap::read() -> bool
{
    try {
        open();
        if (isOpen()) {
            readDB();
            close();
        }
    } catch (std::exception &e) {
        tl::printException(e);
        return true;
    }
    return false;
}

void FeaturesReaderColmap::open()
{
    try {
        QStringList string_list = mFileName.split("|");
        mImageId = mDatabase->ReadImageWithName(string_list[1].toStdString());
        mDatabase->Open(string_list[0].toStdString());
        bOpen = true;
    } catch (...) {
        /// No hay ningun método para ver si se ha abierto correctamente la base de datos
        /// Capturo si hay una excepción y pongo a falso bOpen
        bOpen = false;
    }
}

auto FeaturesReaderColmap::isOpen() const -> bool
{
    return bOpen;
}

void FeaturesReaderColmap::readDB() const
{
    mImageId;
}

void FeaturesReaderColmap::close()
{
    mDatabase->Close();
    bOpen = false;
}



/* ---------------------------------------------------------------------------------- */



class FeaturesWriterColmap
    : public FeaturesWriter
{

public:

    explicit FeaturesWriterColmap(const QString &fileName);
    ~FeaturesWriterColmap() override = default;

// FeaturesWriter interface

public:

    auto write() -> bool override;

private:

    void open();
    auto isOpen() const -> bool;
    void writeDB() const;
    void close();

private:

    std::unique_ptr<colmap::Database> mDatabase;
    bool bOpen = false;
    colmap::Image mImageId;
};



FeaturesWriterColmap::FeaturesWriterColmap(const QString &fileName)
    : FeaturesWriter(fileName),
    mDatabase(new colmap::Database)
{

}

auto FeaturesWriterColmap::write() -> bool
{
    try {

        open();
        if (isOpen()) {
            writeDB();
            close();
        }

    } catch (std::exception &e) {
        tl::printException(e);
        return true;
    }
    return false;
}

void FeaturesWriterColmap::open()
{
    try {
        QStringList string_list = mFileName.split("|");
        mImageId = mDatabase->ReadImageWithName(string_list[1].toStdString());
        mDatabase->Open(string_list[0].toStdString());
        bOpen = true;
    } catch (...) {
        /// No hay ningun método para ver si se ha abierto correctamente la base de datos
        /// Capturo si hay una excepción y pongo a falso bOpen
        bOpen = false;
    }
}

bool FeaturesWriterColmap::isOpen() const
{
    return bOpen;
}

void FeaturesWriterColmap::writeDB() const
{
    mDatabase->WriteKeypoints(mImageId.ImageId(), mKeyPoints);
    mDatabase->WriteDescriptors(mImageId.ImageId(), mDescriptors);
}

void FeaturesWriterColmap::close()
{
    mDatabase->Close();
    bOpen = false;
}





auto FeaturesReaderFactory::createReader(const QString &fileName) -> std::unique_ptr<FeaturesReader>
{
    QString ext = QFileInfo(fileName).suffix();
    std::unique_ptr<FeaturesReader> features_reader;
    if (ext.compare("db", Qt::CaseInsensitive) == 0) {
        features_reader = std::make_unique<FeaturesReaderColmap>(fileName);
    } else {
        throw std::runtime_error("Invalid Features Reader");
    }
    return features_reader;
}




auto FeaturesWriterFactory::createWriter(const QString &fileName) -> std::unique_ptr<FeaturesWriter>
{
    QString ext = QFileInfo(fileName).suffix();
    std::unique_ptr<FeaturesWriter> features_writer;
    if (ext.compare("db", Qt::CaseInsensitive) == 0) {
        features_writer = std::make_unique<FeaturesWriterColmap>(fileName);
    } else {
        throw std::runtime_error("Invalid Features Writer");
    }
    return features_writer;
}




FeaturesIOHandler::FeaturesIOHandler() = default;

auto FeaturesIOHandler::read(const QString &file) -> bool
{
    try {
        mReader = FeaturesReaderFactory::createReader(file);
        return mReader->read();
    } catch (std::exception &e) {
        tl::printException(e);
        return true;
    }
}

auto FeaturesIOHandler::write(const QString &file) -> bool
{
    try {
        mWriter = FeaturesWriterFactory::createWriter(file);
        return mWriter->write();
    } catch (std::exception &e) {
        tl::printException(e);
        return true;
    }
}


} // namespace graphos

