#ifndef INSPECTOR_CAMERA_POSITIONS_PRESENTER_H
#define INSPECTOR_CAMERA_POSITIONS_PRESENTER_H

#include "inspector/ui/import/cameras/CameraPositionsPresenter.h"

namespace inspector
{

namespace ui
{

class CamerasImportView;
class CamerasImportModel;
class HelpDialog;

class CamerasImportPresenterImp
  : public CamerasImportPresenter
{

  Q_OBJECT

public:

  CamerasImportPresenterImp(CamerasImportView *view,
                            CamerasImportModel *model);
  ~CamerasImportPresenterImp() override;

private slots:

  void previewCSV();
  void checkInputCRS(const QString &crs);
  void checkOutputCRS(const QString &crs);

private:

  //bool checkCRS(const QString &crs);

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  CamerasImportView *mView;
  CamerasImportModel *mModel;
  HelpDialog *mHelp;
//  QString mFile;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERA_POSITIONS_PRESENTER_H
