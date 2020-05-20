#include <inspector/core/project.h>

namespace inspector
{

class ProjectFake
  : public ProjectImp
{

public:

  ProjectFake();

  ~ProjectFake() override {}

// Project interface

public:

  bool load(const QString &file) override;

protected:

  QString mProjectFileText;

};

} // end namespace inspector
