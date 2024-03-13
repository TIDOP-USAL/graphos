#ifndef GRAPHOS_TEST_PROJECT_FAKE_H
#define GRAPHOS_TEST_PROJECT_FAKE_H

#include <graphos/core/project.h>

namespace graphos
{

class ProjectFake
  : public ProjectImp
{

public:

  ProjectFake();

  ~ProjectFake() override {}

// Project interface

public:

  void load(const tl::Path &file) override;
  void save(const tl::Path &file) override;

protected:

  QString mProjectFileText;

};

} // end namespace graphos

#endif GRAPHOS_TEST_PROJECT_FAKE_H
