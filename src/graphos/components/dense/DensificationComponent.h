/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_DENSIFICATION_COMPONENT_H
#define GRAPHOS_DENSIFICATION_COMPONENT_H

/* TidopLib */
#include <tidop/core/flags.h>

/* GRAPHOS */
#include "graphos/core/Component.h"

namespace graphos
{

class DensificationProcess;

class DensificationComponent
  : public TaskComponent
{

  Q_OBJECT

public:

    enum class Method
    {
        mvs =  (1 << 0),
        pmvs = (1 << 1),
        smvs = (1 << 2)
    };

public:

  DensificationComponent(Application *application);
  ~DensificationComponent();

  void enableMethod(Method method);
  void disableMethod(Method method);
  bool isEnabled(Method method) const;

private:

  void init();

// ComponentBase

protected:

  void createModel() override;
  void createView() override;
  void createPresenter() override;
  void createCommand() override;
  void update() override;

// TaskComponent

protected slots:

  void onRunning() override;
  void onFinished() override;
  void onFailed() override;

private:

  tl::EnumFlags<Method> mMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(DensificationComponent::Method)

} // namespace graphos


#endif // GRAPHOS_DENSIFICATION_COMPONENT_H
