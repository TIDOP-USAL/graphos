#ifndef INSPECTOR_PROCESS_VIEW_H
#define INSPECTOR_PROCESS_VIEW_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

namespace ui
{


class ProcessView
  : public DialogView
{

public:

  ProcessView();
  ~ProcessView();

signals:

  void run();

};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_PROCESS_VIEW_H
