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
 
#include "AboutModel.h"

#include <tidop/core/app/License.h>
#include <tidop/core/base/Path.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>

namespace graphos
{


AboutModelImp::AboutModelImp(QObject *parent)
  : AboutModel(parent),
    mAppLicense(new tl::AppLicense)
{
    AboutModelImp::init();
}

AboutModelImp::~AboutModelImp()
{
    if (mAppLicense) {
        delete mAppLicense;
        mAppLicense = nullptr;
    }
}

auto AboutModelImp::license() const -> const tl::License&
{
    return *static_cast<tl::License *>(mAppLicense);
}

auto AboutModelImp::readLicense(const QString &license) -> QString
{
    QString licence_text;

    QFile file(license);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        licence_text = file.readAll();
        file.close();
    }

    return licence_text;
}

auto AboutModelImp::begin() const -> const_iterator
{
    return mAppLicense->begin();
}

auto AboutModelImp::end() const -> const_iterator
{
    return mAppLicense->end();
}

/* private */

void AboutModelImp::init()
{
    // Lectura de la licencia y de las licencias de terceros

    QDir plugins_dir = QDir(QCoreApplication::applicationDirPath() + "\\licenses");
    QFile file(plugins_dir.absoluteFilePath("license.json"));
    tl::Path path(plugins_dir.absolutePath().toStdWString());

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QString json = file.readAll();
        file.close();

        QJsonDocument json_document = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject json_object = json_document.object();

        QJsonValue json_value = json_object.value("ProductName");
        if (!json_value.isNull())
            mAppLicense->setProductName(json_value.toString().toStdString());

        json_value = json_object.value("ProductVersion");
        if (!json_value.isNull())
            mAppLicense->setVersion(json_value.toString().toStdString());

        json_value = json_object.value("LicenceFile");
        if (!json_value.isNull()) {
            tl::Path path_license(path);
            path_license.append(json_value.toString().toStdString());
            mAppLicense->setLicenseText(path_license.toString());
        }


        json_value = json_object.value("ThirdPartyLicences");
        if (!json_value.isNull()) {

            tl::License license;

            QJsonArray json_array = json_value.toArray();
            for (const auto &json : json_array) {

                QJsonObject json_object = json.toObject();
                QJsonValue json_value = json_object.value("ProductName");
                if (!json_value.isNull())
                    license.setProductName(json_value.toString().toStdString());

                json_value = json_object.value("ProductVersion");
                if (!json_value.isNull())
                    license.setVersion(json_value.toString().toStdString());

                json_value = json_object.value("LicenceFile");
                if (!json_value.isNull()) {
                    tl::Path path_license(path);
                    path_license.append(json_value.toString().toStdString());
                    license.setLicenseText(path_license.toString());
                }

                mAppLicense->push_back(license);
            }
        }
    }

}

void AboutModelImp::clear()
{

}

} // namespace graphos
