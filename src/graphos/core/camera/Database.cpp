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

#include "graphos/core/camera/Database.h"

#include <tidop/core/exception.h>

#include <QSqlDatabase>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <SQLite/sqlite3.h>

using namespace tl;

namespace graphos
{


DatabaseCameras::DatabaseCameras(const QString &database)
  : mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "database_cameras"))),
    mDatabasePath(database)
{
    mDatabase->setDatabaseName(database);
}

DatabaseCameras::~DatabaseCameras()
{
    QString name = mDatabase->connectionName();

    if (mDatabase) {
        delete mDatabase;
        mDatabase = nullptr;
    }

    QSqlDatabase::removeDatabase(name);
}

void DatabaseCameras::open()
{
    try {

        TL_ASSERT(QFileInfo(mDatabase->databaseName()).exists(), "The camera database does not exist");
        TL_ASSERT(mDatabase->open(), "The camera database does not exist");

    } catch (...){
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception in open");
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

        TL_ASSERT(isOpen(), "Cannot open database");

        QSqlQuery query(QSqlDatabase::database("database_cameras"));
        query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
        query.bindValue(":camera_make", cameraMake);
        if (query.exec()) {
            if (query.next()) {
                exist_camera = true;
            }
        } else {
            QSqlError err = query.lastError();
            tl::Message::error(err.text().toStdString());
        }

        query.finish();

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

        // Application crash. Docker image with Qt version 5.12.8
        //TL_ASSERT(isOpen(), "Cannot open database");
        TL_ASSERT(cameraMake != -1, "Invalid Camera Make");

        //QSqlQuery query(QSqlDatabase::database("database_cameras"));
        //query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera = :id_camera");
        //query.bindValue(":camera_model", cameraModel);
        //query.bindValue(":id_camera", cameraMake);
        //if (query.exec()) {
        //    if (query.next()) {
        //        exist_camera_model = true;
        //    }
        //} else {
        //    QSqlError err = query.lastError();
        //    tl::Message::error(err.text().toStdString());
        //}

        //query.finish();

        sqlite3 *db;
        sqlite3_stmt *stmt;

        int rc = sqlite3_open(mDatabasePath.toStdString().c_str(), &db);
        TL_ASSERT(rc == SQLITE_OK, "Cannot open database: {}", sqlite3_errmsg(db));

        auto sql = "SELECT COUNT(*) FROM models WHERE id_camera = ? AND camera_model LIKE ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            tl::Message::error("Failed to prepare statement: {}", sqlite3_errmsg(db));
            sqlite3_close(db);
            return exist_camera_model;
        }

        sqlite3_bind_int(stmt, 1, cameraMake);
        sqlite3_bind_text(stmt, 2, cameraModel.toStdString().c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            exist_camera_model = count > 0;
        } else {
            tl::Message::error("Query error: {}", sqlite3_errmsg(db));
            exist_camera_model = false;
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

    } catch (...) {
        exist_camera_model = false;
    }

    return exist_camera_model;
}

int DatabaseCameras::cameraMakeId(const QString &cameraMake) const
{
    int id_camera = -1;

    try {

        TL_ASSERT(isOpen(), "Database is not open");

        QSqlQuery query(QSqlDatabase::database("database_cameras"));
        query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
        query.bindValue(":camera_make", cameraMake);
        if (query.exec()) {
            if (query.next()) {
                id_camera = query.value(0).toInt();
            }
        } else {
            QSqlError err = query.lastError();
            tl::Message::error(err.text().toStdString());
        }

        query.finish();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return id_camera;
}

double DatabaseCameras::cameraSensorSize(int cameraMake, const QString &cameraModel) const
{
    double sensor_width_mm = -1;

    try {

        // Application crash. Docker image with Qt version 5.12.8

        //TL_ASSERT(isOpen(), "Database is not open");
        TL_ASSERT(cameraMake != -1, "Invalid Camera Make");

        //QSqlQuery query(QSqlDatabase::database("database_cameras"));
        //query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera = :id_camera");
        //query.bindValue(":camera_model", cameraModel);
        //query.bindValue(":id_camera", cameraMake);
        //if (query.exec()) {
        //    if (query.next()) {
        //        sensor_width_mm = query.value(0).toDouble();
        //    }

        //    TL_ASSERT(sensor_width_mm > 0., "Camera model not found in database");

        //} else {
        //    QSqlError err = query.lastError();
        //    tl::Message::error(err.text().toStdString());
        //}

        //query.finish();

        sqlite3 *db;
        sqlite3_stmt *stmt;

        int rc = sqlite3_open(mDatabasePath.toStdString().c_str(), &db);
        TL_ASSERT(rc == SQLITE_OK, "Cannot open database: {}", sqlite3_errmsg(db));

        auto sql = "SELECT sensor_width FROM models WHERE camera_model LIKE ? AND id_camera = ?";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            tl::Message::error("Failed to prepare statement: {}", sqlite3_errmsg(db));
            sqlite3_close(db);
            return sensor_width_mm;
        }

        sqlite3_bind_text(stmt, 1, cameraModel.toStdString().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, cameraMake);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            sensor_width_mm = sqlite3_column_double(stmt, 0);
        }

        // Limpiar y cerrar
        sqlite3_finalize(stmt);
        sqlite3_close(db);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return sensor_width_mm;
}

} // namespace graphos
