#ifndef INSPECTOR_POINT_CLOUD_EXPORT_PRESENTER_H
#define INSPECTOR_POINT_CLOUD_EXPORT_PRESENTER_H

#include "inspector/ui/export/densemodel/ExportPointCloudPresenter.h"

namespace inspector
{

class PointCloudCSVFormatWidget;

namespace ui
{

class ExportPointCloudView;
class ExportPointCloudModel;


class HelpDialog;

class ExportPointCloudPresenterImp
  : public ExportPointCloudPresenter
{

  Q_OBJECT

public:

  ExportPointCloudPresenterImp(ExportPointCloudView *view,
                               ExportPointCloudModel *model);
  ~ExportPointCloudPresenterImp() override;

// ExportPointCloudPresenter interface

public slots:

  void save() override;
  void setCurrentFormat(const QString &format) override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  ExportPointCloudView *mView;
  ExportPointCloudModel *mModel;
  //PointCloudCSVFormatWidget *mPointCloudCSVFormatWidget;
  HelpDialog *mHelp;
  QString mFile;
  QString mFormat;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_POINT_CLOUD_EXPORT_PRESENTER_H
