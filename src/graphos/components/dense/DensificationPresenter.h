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

#ifndef GRAPHOS_DENSE_PRESENTER_INTERFACE_H
#define GRAPHOS_DENSE_PRESENTER_INTERFACE_H

#include "graphos/core/task/TaskPresenter.h"

namespace graphos
{

class CmvsPmvsWidget;
class SmvsWidget;
class MvsWidget;


/*!
 * \brief The DensificationPresenter class represents the presenter interface for densification tasks.
 */
class DensificationPresenter
  : public TaskPresenter
{

    Q_OBJECT

public:

    DensificationPresenter() = default;
    ~DensificationPresenter() override = default;

    /*!
     * \brief Sets the widget for CMVS/PMVS.
     * \param[in] cmvsPmvs The CMVS/PMVS widget.
     */
    virtual void setCmvsPmvsWidget(std::shared_ptr<CmvsPmvsWidget> cmvsPmvs) = 0;

    /*!
     * \brief setSmvsWidget Sets the widget for SMVS.
     * \param[in] smvs The SMVS widget.
     */
    virtual void setSmvsWidget(std::shared_ptr<SmvsWidget> smvs) = 0;

    /*!
     * \brief setMvsWidget Sets the widget for MVS.
     * \param[in] mvs The MVS widget.
     */
    virtual void setMvsWidget(std::shared_ptr<MvsWidget> mvs) = 0;

public slots:

    /*!
     * \brief Sets the current densifier.
     * \param[in] densifier The densifier to set.
     */
    virtual void setCurrentDensifier(const QString &densifier) = 0;

signals:

    void densification_deleted();

};

} // End namespace graphos

#endif // GRAPHOS_DENSE_PRESENTER_INTERFACE_H
