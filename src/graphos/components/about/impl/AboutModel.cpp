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

#include <tidop/core/licence.h>
#include <tidop/core/path.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>

namespace graphos
{


AboutModelImp::AboutModelImp(QObject *parent)
  : AboutModel(parent),
    mAppLicence(new tl::AppLicence)
{
    AboutModelImp::init();
}

AboutModelImp::~AboutModelImp()
{
    if (mAppLicence) {
        delete mAppLicence;
        mAppLicence = nullptr;
    }
}

auto AboutModelImp::licence() const -> const tl::Licence&
{
    return *static_cast<tl::Licence *>(mAppLicence);
}

auto AboutModelImp::readLicence(const QString& licence) -> QString
{
    QString licence_text;

    QFile file(licence);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (file.isOpen()) {

        licence_text = file.readAll();
        file.close();
    }

    return licence_text;
}

auto AboutModelImp::begin() const -> const_iterator
{
    return mAppLicence->begin();
}

auto AboutModelImp::end() const -> const_iterator
{
    return mAppLicence->end();
}

/* private */

void AboutModelImp::init()
{
    // Lectura de la licencia y de las licencias de terceros

    QDir plugins_dir = QDir(QCoreApplication::applicationDirPath() + "\\licenses");
    QFile file(plugins_dir.absoluteFilePath("licence.json"));
    tl::Path path(plugins_dir.absolutePath().toStdWString());

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (file.isOpen()) {

        QString json = file.readAll();
        file.close();

        QJsonDocument json_document = QJsonDocument::fromJson(json.toUtf8());
        QJsonObject json_object = json_document.object();

        QJsonValue json_value = json_object.value("ProductName");
        if (!json_value.isNull())
            mAppLicence->setProductName(json_value.toString().toStdString());

        json_value = json_object.value("ProductVersion");
        if (!json_value.isNull())
            mAppLicence->setVersion(json_value.toString().toStdString());

        json_value = json_object.value("LicenceFile");
        if (!json_value.isNull()) {
            tl::Path path_licence(path);
            path_licence.append(json_value.toString().toStdString());
            mAppLicence->setText(path_licence.toString());
        }


        json_value = json_object.value("ThirdPartyLicences");
        if (!json_value.isNull()) {

            tl::Licence licence;

            QJsonArray json_array = json_value.toArray();
            for (const auto &json : json_array) {

                QJsonObject json_object = json.toObject();
                QJsonValue json_value = json_object.value("ProductName");
                if (!json_value.isNull())
                    licence.setProductName(json_value.toString().toStdString());

                json_value = json_object.value("ProductVersion");
                if (!json_value.isNull())
                    licence.setVersion(json_value.toString().toStdString());

                json_value = json_object.value("LicenceFile");
                if (!json_value.isNull()) {
                    tl::Path path_licence(path);
                    path_licence.append(json_value.toString().toStdString());
                    licence.setText(path_licence.toString());
                }

                mAppLicence->push_back(licence);
            }
        }
    }

}

void AboutModelImp::clear()
{

}

} // namespace graphos
