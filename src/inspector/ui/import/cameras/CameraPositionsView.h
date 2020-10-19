#ifndef INSPECTOR_CAMERA_POSITIONS_VIEW_INTERFACE_H
#define INSPECTOR_CAMERA_POSITIONS_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QStandardItemModel;

namespace inspector
{

namespace ui
{

class CamerasImportView
  : public IDialogView
{

  Q_OBJECT

public:

  CamerasImportView(QWidget *parent) : IDialogView(parent) {}
  ~CamerasImportView() override = default;

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
  void imageColumnChange(const QString &);
  void imageFieldIdChanged(int);
  //void xColumnChange(const QString &);
  void xFieldIdChanged(int);
  //void yColumnChange(const QString &);
  void yFieldIdChanged(int);
  //void zColumnChange(const QString &);
  void zFieldIdChanged(int);
  //void qxColumnChange(const QString &);
  //void qyColumnChange(const QString &);
  //void qzColumnChange(const QString &);
  //void qwColumnChange(const QString &);
  //void omegaColumnChange(const QString &);
  //void phiColumnChange(const QString &);
  //void kappaColumnChange(const QString &);
  //void yawColumnChange(const QString &);
  //void pitchColumnChange(const QString &);
  //void rollColumnChange(const QString &);
  void crsInputChanged(QString);
  void crsOutputChanged(QString);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERA_POSITIONS_VIEW_INTERFACE_H
