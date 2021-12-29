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

#ifndef GRAPHOS_IMPORT_CAMERAS_MODEL_INTERFACE_H
#define GRAPHOS_IMPORT_CAMERAS_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

class QStandardItemModel;

namespace graphos
{

class ImportCamerasModel
  : public Model
{

  Q_OBJECT

public:

  ImportCamerasModel(QObject *parent = nullptr) : Model(parent) {}
  ~ImportCamerasModel() override = default;

  virtual QStandardItemModel *itemModelCSV() = 0;
  virtual QStandardItemModel *itemModelFormatCameras() = 0;
  virtual bool checkCRS(const QString &crs) = 0;
  virtual QString outputCRS() const = 0;

public slots:

  virtual void setCsvFile(const QString &csv) = 0;
  virtual void previewImportCameras() = 0;
  virtual void previewImportCamerasFormated() = 0;
  virtual void loadCameras() = 0;

  virtual void setFieldNamesFromFirstRow(bool active) = 0;
  virtual void setDelimiter(const QString &delimiter) = 0;
  virtual void setInitialLine(int iniLine) = 0;
  virtual void setImageFieldId(int id) = 0;
  virtual void setXFieldId(int id) = 0;
  virtual void setYFieldId(int id) = 0;
  virtual void setZFieldId(int id) = 0;
  virtual void setRotationType(const QString &rotationType) = 0;
  virtual void setQxFieldId(int id) = 0;
  virtual void setQyFieldId(int id) = 0;
  virtual void setQzFieldId(int id) = 0;
  virtual void setQwFieldId(int id) = 0;
  virtual void setYawFieldId(int id) = 0;
  virtual void setPitchFieldId(int id) = 0;
  virtual void setRollFieldId(int id) = 0;
  virtual void setOmegaFieldId(int id) = 0;
  virtual void setPhiFieldId(int id) = 0;
  virtual void setKappaFieldId(int id) = 0;
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

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_MODEL_INTERFACE_H
