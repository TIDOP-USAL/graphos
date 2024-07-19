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

#include "FeatureMatchingModel.h"

#include <tidop/core/msg/message.h>

#include <colmap/base/database.h>

#include <QFileInfo>
#include <QSettings>
#include <tidop/geospatial/crs.h>

namespace graphos
{

FeatureMatchingModelImp::FeatureMatchingModelImp(Project *project,
                                                 QObject *parent)
  : FeatureMatchingModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
                            QSettings::UserScope,
                            qApp->organizationName(),
                            qApp->applicationName()))
{
    FeatureMatchingModelImp::init();
}

FeatureMatchingModelImp::~FeatureMatchingModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void FeatureMatchingModelImp::init()
{
}

void FeatureMatchingModelImp::clear()
{

}

auto FeatureMatchingModelImp::featureMatching() const -> std::shared_ptr<FeatureMatching>
{
    return mProject->featureMatching();
}

void FeatureMatchingModelImp::setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching)
{
    mProject->setFeatureMatching(featureMatching);
}

void FeatureMatchingModelImp::setFeatureMatchingReport(const FeatureMatchingReport &report)
{
    mProject->setFeatureMatchingReport(report);
}

auto FeatureMatchingModelImp::database() const -> tl::Path
{
    return mProject->database();
}

auto FeatureMatchingModelImp::useCuda() const -> bool
{
    return mSettings->value("UseCuda", true).toBool();
}

auto FeatureMatchingModelImp::spatialMatching() const -> bool
{
    for (const auto &image : mProject->images()) {
        if (!image.second.cameraPose().isEmpty()) {
            return true;
        }
    }

    return false;
}

auto FeatureMatchingModelImp::geodeticCoordinates() const -> bool
{
    for (const auto &image : mProject->images()) {
        if (!image.second.cameraPose().isEmpty()) {
            auto epsg_code = image.second.cameraPose().crs().toStdString();
            tl::Crs crs(epsg_code);
            return crs.isGeographic();
        }
    }

    return false;
}

void FeatureMatchingModelImp::writeMatchPairs()
{

    colmap::Database database(mProject->database().toString());
    std::vector<colmap::Image> db_images = database.ReadAllImages();
    colmap::image_t colmap_image_id_l = 0;
    colmap::image_t colmap_image_id_r = 0;

    for (size_t i = 0; i < db_images.size(); i++) {

        colmap_image_id_l = db_images[i].ImageId();
        tl::Path colmap_image1(db_images[i].Name());

        for (size_t j = 0; j < i; j++) {
            colmap_image_id_r = db_images[j].ImageId();

            colmap::FeatureMatches matches = database.ReadMatches(colmap_image_id_l, colmap_image_id_r);

            if (!matches.empty()) {
                
                tl::Path colmap_image2(db_images[j].Name());

                bool find_id_1 = false;
                bool find_id_2 = false;
                size_t image_id_1 = 0;
                size_t image_id_2 = 0;

                for (const auto &image_pair : mProject->images()) {

                    tl::Path image(image_pair.second.path().toStdString());

                    if (!find_id_1 && image.equivalent(colmap_image1)) {
                        find_id_1 = true;
                        image_id_1 = image_pair.first;
                    }

                    if (!find_id_2 && image.equivalent(colmap_image2)) {
                        find_id_2 = true;
                        image_id_2 = image_pair.first;
                    }

                    if (find_id_1 && find_id_2) {
                        mProject->addMatchesPair(image_id_1, image_id_2);
                    }
                }

            }

        }

    }

    database.Close();

}

void FeatureMatchingModelImp::cleanProject()
{
    colmap::Database database(mProject->database().toString());
    database.ClearMatches();
    database.ClearTwoViewGeometries();
    database.Close();
    mProject->removeMatchesPair();
}

size_t FeatureMatchingModelImp::imagesSize() const
{
    return mProject->imagesCount();
}

} // End namespace graphos
