#ifndef INSPECTOR_IMPORT_CAMERAS_MODEL_H
#define INSPECTOR_IMPORT_CAMERAS_MODEL_H

#include "inspector/ui/import/cameras/ImportCamerasModel.h"


namespace inspector
{

class Project;

namespace ui
{

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

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMPORT_CAMERAS_MODEL_H
