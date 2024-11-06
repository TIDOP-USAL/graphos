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

#ifndef GRAPHOS_DEM_VIEW_INTERFACE_H
#define GRAPHOS_DEM_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{


/*!
 * \brief The DemView class represents the view for Digital elevation Model (DEM) tasks.
 */
class DemView
  : public DialogView
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for DtmView.
     * \param[in] parent The parent QWidget (default is nullptr).
     */
    DemView(QWidget *parent = nullptr) : DialogView(parent) {}

    /*!
     * \brief Destructor for DtmView.
     */
    ~DemView() override = default;

    /*!
     * \brief gsd Retrieves the Ground Sample Distance (GSD).
     * \return The GSD value.
     */
    virtual auto gsd() const -> double = 0;

    /*!
     * \brief isDsmEnable Checks if the Digital Surface Model (DSM) is enabled.
     * \return True if the MDS is enabled, otherwise false.
     */
    virtual auto isDsmEnable() const -> bool = 0;

    /*!
     * \brief isDtmEnable Checks if the Digital Terrain Model (DTM) is enabled.
     * \return True if the DTM is enabled, otherwise false.
     */
    virtual auto isDtmEnable() const -> bool = 0;

    virtual auto crs() const -> QString = 0;

signals:

    /*!
     * \brief run Signal emitted to start the DTM task.
     */
    void run();

    void crs_change();
    void select_crs();

public slots:

    /*!
     * \brief setGsd Sets the Ground Sample Distance (GSD).
     * \param[in] gsd The GSD value to set.
     */
    virtual void setGsd(double gsd) = 0;

    /*!
     * \brief enableDsm Enables or disables the Digital Surface Model (DSM).
     * \param[in] enable True to enable, false to disable (default is true).
     */
    virtual void enableDsm(bool enable = true) = 0;

    /*!
     * \brief enableDtm Enables or disables the Digital Terrain Model (DTM).
     * \param[in] enable True to enable, false to disable (default is true).
     */
    virtual void enableDtm(bool enable = true) = 0;

    virtual void setCrs(const QString &crs) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DEM_VIEW_INTERFACE_H
