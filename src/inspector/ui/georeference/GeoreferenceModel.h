#ifndef INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QStandardItemModel;

namespace inspector
{

namespace ui
{

class GeoreferenceModel
  : public IModel
{

  Q_OBJECT

public:

  GeoreferenceModel(QObject *parent = nullptr) : IModel(parent) {}
  ~GeoreferenceModel() override = default;

  virtual QStandardItemModel *itemModel() = 0;

public slots:

  virtual void importPositionsFromCSV(const QString &csv,
                                      const QString &split) = 0;
  virtual void loadCameraPositions() = 0;

signals:

  void csvHeader(QStringList);
  void imageColumn(QString);
  void xColumn(QString);
  void yColumn(QString);
  void zColumn(QString);
  void qxColumn(QString);
  void qyColumn(QString);
  void qzColumn(QString);
  void qwColumn(QString);
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_MODEL_INTERFACE_H
