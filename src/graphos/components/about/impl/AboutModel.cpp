/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/
 
#include "AboutModel.h"

#include <tidop/core/licence.h>
#include <tidop/core/path.h>

#include <QFile>
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

const tl::Licence &AboutModelImp::graphosLicence() const
{
  return *static_cast<tl::Licence *>(mAppLicence);
}

QString AboutModelImp::readLicence(const QString &licence)
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

AboutModelImp::const_iterator AboutModelImp::begin() const
{
  return mAppLicence->begin();
}

AboutModelImp::const_iterator AboutModelImp::end() const
{
  return mAppLicence->end();
}

/* private */

void AboutModelImp::init()
{
  // Lectura de la licencia y de las licencias de terceros

  QString json;
  QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "\\licenses");
  //QFile file("C:\\Desarrollo\\instaladores\\inspector\\licenses\\licence.json");
  QFile file(pluginsDir.absoluteFilePath("licence.json"));
  
  tl::Path path(pluginsDir.absolutePath().toStdString());

  file.open(QIODevice::ReadOnly | QIODevice::Text);

  if (file.isOpen()) {

    json = file.readAll();
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
      mAppLicence->setType(path_licence.toString()); /// Cambiar nombre de metodo
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
          licence.setType(path_licence.toString());
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
