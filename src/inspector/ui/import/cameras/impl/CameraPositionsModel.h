#ifndef INSPECTOR_CAMERA_POSITIONS_MODEL_H
#define INSPECTOR_CAMERA_POSITIONS_MODEL_H

#include "inspector/ui/import/cameras/CameraPositionsModel.h"


namespace inspector
{

class Project;

namespace ui
{

class CamerasImportModelImp
  : public CamerasImportModel
{

  Q_OBJECT

public:

  CamerasImportModelImp(Project *project,
                       QObject *parent = nullptr);
  ~CamerasImportModelImp() override = default;

// CamerasImportModel interface

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
  void setImageFieldId(int id) override;
  void setXFieldId(int id) override;
  void setYFieldId(int id) override;
  void setZFieldId(int id) override;
  void setInputCRS(const QString &crs) override;
  void setOutputCRS(const QString &crs) override;
  void importCameras() override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QString mCsvFile;
  bool bFieldNamesFromFirstRow;
  QString mDelimiter;
  QString mInputCrs;
  QString mOutputCrs;
  std::map<QString, int> mFieldIds;
  QStandardItemModel *mItemModelCSV;
  QStandardItemModel *mItemModelCameras;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERA_POSITIONS_MODEL_H