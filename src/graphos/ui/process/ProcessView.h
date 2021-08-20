#ifndef GRAPHOS_PROCESS_VIEW_H
#define GRAPHOS_PROCESS_VIEW_H

#include "graphos/interfaces/mvp.h"


namespace graphos
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

} // namespace graphos


#endif // GRAPHOS_PROCESS_VIEW_H
