#ifndef GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H
#define GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H

#include "graphos/ui/export/orientations/ExportOrientationsPresenter.h"

namespace graphos
{

class NvmFormatWidget;
class BundlerFormatWidget;
class MveFormatWidget;
class OriTxtFormatWidget;

namespace ui
{

class ExportOrientationsView;
class ExportOrientationsModel;
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

// Presenter interface

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
  MveFormatWidget *mMveFormatWidget;
  OriTxtFormatWidget *mOriTxtFormatWidget;
  HelpDialog *mHelp;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_PRESENTER_H
