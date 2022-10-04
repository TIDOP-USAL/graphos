#ifndef GRAPHOS_DTM_MODEL_H
#define GRAPHOS_DTM_MODEL_H

#include "graphos/components/dtm/DTMModel.h"

namespace graphos
{

class Project;
class Feature;

class DtmModelImp
  : public DtmModel
{

public:

  DtmModelImp(Project *project,
              QObject *parent = nullptr);
  ~DtmModelImp() override = default;

// DtmModel interface

  std::array<double, 3> offset() const override;
  tl::Path projectPath() const override;
  tl::Path denseModel() const override;
  QString crs() const override;

  std::shared_ptr<Dtm> dtmMethod() const override;
  void setDtmMethod(const std::shared_ptr<Dtm> &dtm) override;
  tl::Path dtmPath() const override;
  void setDtmPath(const tl::Path &dtmPath) override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};


} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_H
