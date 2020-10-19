#ifndef INSPECTOR_CAMERA_POSITIONS_MODEL_INTERFACE_H
#define INSPECTOR_CAMERA_POSITIONS_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QStandardItemModel;

namespace inspector
{

namespace ui
{

class CamerasImportModel
  : public IModel
{

  Q_OBJECT

public:

  CamerasImportModel(QObject *parent = nullptr) : IModel(parent) {}
  ~CamerasImportModel() override = default;

  virtual QStandardItemModel *itemModelCSV() = 0;
  virtual QStandardItemModel *itemModelFormatCameras() = 0;
  virtual bool checkCRS(const QString &crs) = 0;

public slots:

  virtual void setCsvFile(const QString &csv) = 0;
  virtual void previewImportCameras() = 0;
  virtual void previewImportCamerasFormated() = 0;
  virtual void loadCameras() = 0;

  virtual void setFieldNamesFromFirstRow(bool active) = 0;
  virtual void setDelimiter(const QString &delimiter) = 0;
  virtual void setImageFieldId(int id) = 0;
  virtual void setXFieldId(int id) = 0;
  virtual void setYFieldId(int id) = 0;
  virtual void setZFieldId(int id) = 0;
  virtual void setInputCRS(const QString &crs) = 0;
  virtual void setOutputCRS(const QString &crs) = 0;
  virtual void importCameras() = 0;

signals:

  void csvHeader(QStringList);
  void imageColumn(QString);
  void xColumn(QString);
  void yColumn(QString);
  void zColumn(QString);
  void qxColumn(QString);
  void qyColumn(QString);
  void qzColumn(QString);
  void qwColumn(QString);
  void omegaColumn(QString);
  void phiColumn(QString);
  void kappaColumn(QString);
  void yawColumn(QString);
  void pitchColumn(QString);
  void rollColumn(QString);

  void parseOk(bool);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H
