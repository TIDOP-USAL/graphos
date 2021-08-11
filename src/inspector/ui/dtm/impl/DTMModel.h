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
  QString crs() const override;

  std::shared_ptr<Dtm> dtmMethod() const override;
  void setDtmMethod(const std::shared_ptr<Dtm> &dtm) override;
  QString dtmPath() const override;
  void setDtmPath(const QString &dtmPath) override;

// Model interface

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
