#ifndef INSPECTOR_GEOREFERENCE_MODEL_H
#define INSPECTOR_GEOREFERENCE_MODEL_H

#include "inspector/ui/georeference/GeoreferenceModel.h"


namespace inspector
{

class Project;

namespace ui
{

class GeoreferenceModelImp
  : public GeoreferenceModel
{

  Q_OBJECT

public:

  GeoreferenceModelImp(Project *project,
                       QObject *parent = nullptr);
  ~GeoreferenceModelImp() override = default;

// GeoreferenceModel interface

public:

  QStandardItemModel *itemModel() override;

public slots:

  void loadCSV(const QString &csv, const QString &split = ";") override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QStandardItemModel *mItemModel;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_MODEL_H
