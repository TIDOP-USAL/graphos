#ifndef INSPECTOR_IMPORT_CAMERAS_VIEW_INTERFACE_H
#define INSPECTOR_IMPORT_CAMERAS_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QStandardItemModel;

namespace inspector
{

namespace ui
{

class ImportCamerasView
  : public DialogView
{

  Q_OBJECT

public:

  ImportCamerasView(QWidget *parent) : DialogView(parent) {}
  ~ImportCamerasView() override = default;

  virtual QString delimiter() const = 0;

public slots:

  virtual void setItemModel(QStandardItemModel *model) = 0;
  virtual void setItemModelFormatCameras(QStandardItemModel *model) = 0;
  virtual void setTableHeader(const QStringList &header) = 0;
  virtual void setImageColumn(const QString &imageColumn) = 0;
  virtual void setXColumn(const QString &xColumn) = 0;
  virtual void setYColumn(const QString &yColumn) = 0;
  virtual void setZColumn(const QString &zColumn) = 0;
  virtual void setQxColumn(const QString &qxColumn) = 0;
  virtual void setQyColumn(const QString &qyColumn) = 0;
  virtual void setQzColumn(const QString &qzColumn) = 0;
  virtual void setQwColumn(const QString &qwColumn) = 0;
  virtual void setOmegaColumn(const QString &omegaColumn) = 0;
  virtual void setPhiColumn(const QString &phiColumn) = 0;
  virtual void setKappaColumn(const QString &kappaColumn) = 0;
  virtual void setYawColumn(const QString &yawColumn) = 0;
  virtual void setPitchColumn(const QString &pitchColumn) = 0;
  virtual void setRollColumn(const QString &rollColumn) = 0;
  virtual void setParseOk(bool parseOk) = 0;
  virtual void setValidInputCRS(bool valid) = 0;
  virtual void setValidOutputCRS(bool valid) = 0;

signals:

  void previewCSV();
  void delimiterChanged(QString);
  void loadFieldNamesFromFirstRow(bool);
  void skipLines(int);
  void imageColumnChange(const QString &);
  void imageFieldIdChanged(int);
  void xFieldIdChanged(int);
  void yFieldIdChanged(int);
  void zFieldIdChanged(int);
  void rotationChange(QString);
  void qxFieldChange(int);
  void qyFieldChange(int);
  void qzFieldChange(int);
  void qwFieldChange(int);
  void omegaFieldChange(int);
  void phiFieldChange(int);
  void kappaFieldChange(int);
  void yawFieldChange(int);
  void pitchFieldChange(int);
  void rollFieldChange(int);
  void crsInputChanged(QString);
  void crsOutputChanged(QString);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMPORT_CAMERAS_VIEW_INTERFACE_H
