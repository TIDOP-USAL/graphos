#ifndef INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QStandardItemModel;

namespace inspector
{

namespace ui
{

class GeoreferenceView
  : public IDialogView
{

  Q_OBJECT

public:

  GeoreferenceView(QWidget *parent) : IDialogView(parent) {}
  ~GeoreferenceView() override = default;

  virtual QString orientationFile() const = 0;

public slots:

  virtual void setProjectPath(const QString &path) = 0;
  virtual void setItemModel(QStandardItemModel *model) = 0;
  virtual void setTableHeader(const QStringList &header) = 0;
  virtual void setImageColumn(const QString &imageColumn) = 0;
  virtual void setXColumn(const QString &xColumn) = 0;
  virtual void setYColumn(const QString &yColumn) = 0;
  virtual void setZColumn(const QString &zColumn) = 0;
  virtual void setQxColumn(const QString &qxColumn) = 0;
  virtual void setQyColumn(const QString &qyColumn) = 0;
  virtual void setQzColumn(const QString &qzColumn) = 0;
  virtual void setQwColumn(const QString &qwColumn) = 0;

signals:

  void loadCSV(const QString &, const QString &);
  void imageColumnChange(const QString &);
  void xColumnChange(const QString &);
  void yColumnChange(const QString &);
  void zColumnChange(const QString &);
  void qxColumnChange(const QString &);
  void qyColumnChange(const QString &);
  void qzColumnChange(const QString &);
  void qwColumnChange(const QString &);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H
