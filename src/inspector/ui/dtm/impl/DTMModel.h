#ifndef INSPECTOR_DTM_MODEL_H
#define INSPECTOR_DTM_MODEL_H

#include "inspector/ui/dtm/DTMModel.h"

namespace inspector
{

class Project;
class Feature;

namespace ui
{

class DtmModelImp
  : public DtmModel
{

public:

  DtmModelImp(Project *project,
              QObject *parent = nullptr);
  ~DtmModelImp() override = default;

// DtmModel interface

  std::array<double, 3> offset() const override;
  QString projectPath() const override;
  QString denseModel() const override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;



};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_MODEL_H
