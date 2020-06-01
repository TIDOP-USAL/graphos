#include "LoadImagesProcess.h"

#include <tidop/core/messages.h>

#include <colmap/util/bitmap.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

namespace inspector
{

LoadImagesProcess::LoadImagesProcess(const QString &image)
  : mImage(image)
{

}

void inspector::LoadImagesProcess::run()
{
  try {
    tl::Chrono chrono("Load Images");
    chrono.run();

    QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
    QString database_cameras_path = qApp->applicationDirPath();
    database_cameras_path.append("/cameras.db");
    database_cameras.setDatabaseName(database_cameras_path);
    bool db_open = false;
    if (QFileInfo(database_cameras_path).exists()){
      db_open = database_cameras.open();
    } else {
      msgError("The camera database does not exist");
    }

    colmap::Bitmap bitmap;

    Image image(mImage);

    uint64_t time = chrono.stop();
    msgInfo("[Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
    msgError(e.what());
  }
}

QString LoadImagesProcess::image() const
{
  return mImage;
}

void LoadImagesProcess::setImage(const QString &image)
{
  mImage = image;
}

} // inspector
