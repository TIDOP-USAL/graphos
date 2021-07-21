#ifndef INSPECTOR_EXPORT_ORIENTATIONS_VIEW_H
#define INSPECTOR_EXPORT_ORIENTATIONS_VIEW_H

#include "inspector/ui/export/orientations/ExportOrientationsView.h"

class QLabel;
class QComboBox;
class QDialogButtonBox;
class QGridLayout;

namespace inspector
{

namespace ui
{


class ExportOrientationsViewImp
  : public ExportOrientationsView
{

  Q_OBJECT

public:

  ExportOrientationsViewImp(QWidget *parent = nullptr);
  ~ExportOrientationsViewImp() override;

// ExportOrientationsView interface

public:

  void addFormatWidget(QWidget *formatWidget) override;
  QString format() const override;

public slots:

  void setCurrentFormat(const QString &format) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLabel *mLabelFormat;
  QComboBox *mComboBoxFormat;
  QGridLayout *mGridLayoutFormat;
  QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_VIEW_H
