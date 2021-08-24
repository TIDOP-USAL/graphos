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

#ifndef GRAPHOS_IMPORT_CAMERAS_MODEL_H
#define GRAPHOS_IMPORT_CAMERAS_MODEL_H

#include "graphos/ui/import/cameras/ImportCamerasModel.h"


namespace graphos
{

class Project;

class ImportCamerasModelImp
  : public ImportCamerasModel
{

  Q_OBJECT

public:

  ImportCamerasModelImp(Project *project,
                        QObject *parent = nullptr);
  ~ImportCamerasModelImp() override = default;

// ImportCamerasModel interface

public:

  QStandardItemModel *itemModelCSV() override;
  QStandardItemModel *itemModelFormatCameras() override;
  bool checkCRS(const QString &crs) override;
  QString outputCRS() const override;

public slots:

  void setCsvFile(const QString &csv) override;
  void previewImportCameras() override;
  void previewImportCamerasFormated() override;
  void loadCameras() override;

  void setFieldNamesFromFirstRow(bool active) override;
  void setDelimiter(const QString &delimiter) override;
  void setInitialLine(int iniLine) override;
  void setImageFieldId(int id) override;
  void setXFieldId(int id) override;
  void setYFieldId(int id) override;
  void setZFieldId(int id) override;
  void setRotationType(const QString &rotationType) override;
  void setQxFieldId(int id) override;
  void setQyFieldId(int id) override;
  void setQzFieldId(int id) override;
  void setQwFieldId(int id) override;
  void setYawFieldId(int id) override;
  void setPitchFieldId(int id) override;
  void setRollFieldId(int id) override;
  void setOmegaFieldId(int id) override;
  void setPhiFieldId(int id) override;
  void setKappaFieldId(int id) override;
  void setInputCRS(const QString &crs) override;
  void setOutputCRS(const QString &crs) override;
  void importCameras() override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QString mCsvFile;
  bool bFieldNamesFromFirstRow;
  QString mDelimiter;
  int mIniLine;
  QString mInputCrs;
  QString mOutputCrs;
  QString mRotationType;
  std::map<QString, int> mFieldIds;
  QStandardItemModel *mItemModelCSV;
  QStandardItemModel *mItemModelCameras;
};

} // namespace graphos

#endif // GRAPHOS_IMPORT_CAMERAS_MODEL_H
