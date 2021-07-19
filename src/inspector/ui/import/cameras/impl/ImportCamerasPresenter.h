#ifndef INSPECTOR_IMPORT_CAMERAS_PRESENTER_H
#define INSPECTOR_IMPORT_CAMERAS_PRESENTER_H

#include "inspector/ui/import/cameras/ImportCamerasPresenter.h"

namespace inspector
{

namespace ui
{

class ImportCamerasView;
class ImportCamerasModel;
class HelpDialog;

class ImportCamerasPresenterImp
  : public ImportCamerasPresenter
{

  Q_OBJECT

public:

  ImportCamerasPresenterImp(ImportCamerasView *view,
                            ImportCamerasModel *model);
  ~ImportCamerasPresenterImp() override;

private slots:

  void previewCSV();
  void checkInputCRS(const QString &crs);
  void checkOutputCRS(const QString &crs);

private:

  //bool checkCRS(const QString &crs);

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  ImportCamerasView *mView;
  ImportCamerasModel *mModel;
  HelpDialog *mHelp;
//  QString mFile;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_IMPORT_CAMERAS_PRESENTER_H
