#ifndef INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_H
#define INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_H

#include "inspector/ui/export/orientations/ExportOrientations.h"

namespace inspector
{

class NvmFormatWidget;
class BundlerFormatWidget;

namespace ui
{

class HelpDialog;

class ExportOrientationsPresenterImp
  : public ExportOrientationsPresenter
{

public:

  ExportOrientationsPresenterImp(ExportOrientationsView *view,
                                 ExportOrientationsModel *model);
  ~ExportOrientationsPresenterImp() override;

// ExportOrientationsPresenter interface

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

  ExportOrientationsView *mView;
  ExportOrientationsModel *mModel;
  NvmFormatWidget *mNvmFormatWidget;
  BundlerFormatWidget *mBundlerFormatWidget;
  HelpDialog *mHelp;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_PRESENTER_H
