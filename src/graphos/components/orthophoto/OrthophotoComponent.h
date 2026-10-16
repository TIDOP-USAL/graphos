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

#pragma once

/* TidopLib */
#include <tidop/core/base/flags.h>

/* GRAPHOS */
#include "graphos/framework/component/TaskComponent.h"

namespace graphos
{

class OrthophotoComponent
  : public TaskComponent
{

    Q_OBJECT

public:

    /*!
     * \brief Different densification methods.
     */
    enum class Method
    {
        rlb = (1 << 0),  // Robust Local Blending
        sbb = (1 << 1)   // Seam-based Blending
    };

public:

    OrthophotoComponent(Application *application);
    ~OrthophotoComponent() override;

#ifdef GRAPHOS_GUI

    /*!
     * \brief Enables a orthomosaic method.
     * \param[in] method The method to enable.
     */
    void enableMethod(Method method);

    /*!
     * \brief Disables a orthomosaic method.
     * \param[in] method The method to disable.
     */
    void disableMethod(Method method);

    /*!
     * \brief Checks if a orthomosaic method is enabled.
     * \param[in] method The method to check.
     * \return True if the method is enabled, otherwise false.
     */
    bool isEnabled(Method method) const;

#endif // GRAPHOS_GUI

public slots:

    void setCrs(const QString &crs);

private:

    void init();

signals:

    void select_crs();
    void ortho_deleted();

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
ALLOW_BITWISE_FLAG_OPERATIONS(OrthophotoComponent::Method)

} // namespace graphos