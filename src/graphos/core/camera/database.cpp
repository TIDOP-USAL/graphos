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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/camera/database.h"

#include <tidop/core/exception.h>
#include <tidop/core/messages.h>

#include <QSqlDatabase>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

namespace graphos
{


DatabaseCameras::DatabaseCameras(QString database)
  : mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")))
{
  mDatabase->setDatabaseName(std::move(database));
}

DatabaseCameras::~DatabaseCameras()
{
  if (mDatabase) {
    delete mDatabase;
    mDatabase = nullptr;
  }
}

void DatabaseCameras::open()
{
  bool db_open = false;
  if (QFileInfo(mDatabase->databaseName()).exists()) {
    db_open = mDatabase->open();
  } else {
    msgError("The camera database does not exist");
  }
}

bool DatabaseCameras::isOpen() const
{
  return mDatabase->isOpen();
}

void DatabaseCameras::close()
{
  mDatabase->close();
}

bool DatabaseCameras::existCameraMakeId(const QString &cameraMake) const
{
  bool exist_camera = false;

  try {

    TL_ASSERT(isOpen(), "Database is not open")

    QSqlQuery query;
    query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
    query.bindValue(":camera_make", cameraMake);
    if (query.exec()) {
      while (query.next()) {
        exist_camera = true;
      }
    } else {
      QSqlError err = query.lastError();
      throw err.text().toStdString();
    }

  } catch (...) {
    exist_camera = false;
  }

  return exist_camera;
}

bool DatabaseCameras::existCameraModel(int cameraMake, 
                                       const QString &cameraModel) const
{
  bool exist_camera_model = false;

  try {

    TL_ASSERT(isOpen(), "Database is not open")
    TL_ASSERT(cameraMake != -1, "Invalid Camera Make")

    QSqlQuery query;
    query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
    query.bindValue(":camera_model", cameraModel);
    query.bindValue(":id_camera", cameraMake);
    if (query.exec()) {
      while (query.next()) {
        exist_camera_model = true;
        break;
      }

    }

  } catch (...) {
    exist_camera_model = false;
  }

  return exist_camera_model;
}

int DatabaseCameras::cameraMakeId(const QString &cameraMake) const
{
  int id_camera = -1;

  try {

    TL_ASSERT(isOpen(), "Database is not open")

    QSqlQuery query;
    query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
    query.bindValue(":camera_make", cameraMake);
    if (query.exec()) {
      while (query.next()) {
        id_camera = query.value(0).toInt();
      }
    } else {
      QSqlError err = query.lastError();
      throw err.text().toStdString();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return id_camera;
}

double DatabaseCameras::cameraSensorSize(int cameraMake, const QString &cameraModel) const
{
  double sensor_width_mm = -1;

  try {

    TL_ASSERT(isOpen(), "Database is not open")
    TL_ASSERT(cameraMake != -1, "Invalid Camera Make")

    QSqlQuery query;
    query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
    query.bindValue(":camera_model", cameraModel);
    query.bindValue(":id_camera", cameraMake);
    if (query.exec()) {
      while (query.next()) {
        sensor_width_mm = query.value(0).toDouble();
      }

      TL_ASSERT(sensor_width_mm > 0., "Camera model not found in database");

    } else {
      QSqlError err = query.lastError();
      throw err.text().toStdString();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return sensor_width_mm;
}

} // namespace graphos
