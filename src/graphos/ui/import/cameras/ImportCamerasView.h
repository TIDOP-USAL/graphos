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

#ifndef GRAPHOS_IMPORT_CAMERAS_VIEW_INTERFACE_H
#define GRAPHOS_IMPORT_CAMERAS_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

class QStandardItemModel;

namespace graphos
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
  virtual void setOutputCRS(const QString &crs) = 0;

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

} // namespace graphos

#endif // GRAPHOS_IMPORT_CAMERAS_VIEW_INTERFACE_H
