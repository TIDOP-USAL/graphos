#ifndef GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H
#define GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H

#include "graphos/ui/export/densemodel/ExportPointCloudPresenter.h"

namespace graphos
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

// Presenter interface

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

} // namespace graphos

#endif // GRAPHOS_POINT_CLOUD_EXPORT_PRESENTER_H
