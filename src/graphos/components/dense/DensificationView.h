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

#ifndef GRAPHOS_DENSE_VIEW_INTERFACE_H
#define GRAPHOS_DENSE_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"

class QGridLayout;
class QDialogButtonBox;
class QComboBox;
class QLabel;

namespace graphos
{


/*!
 * \brief The DensificationView class represents the view interface for densification tasks.
 */
class DensificationView
  : public DialogView
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for DensificationView.
     * \param[in] parent The parent QWidget (default is nullptr).
     */
    DensificationView(QWidget *parent) : DialogView(parent) {}

    /*!
     * \brief Destructor for DensificationView.
     */
    ~DensificationView() override = default;

    /*!
     * \brief currentDensificationMethod Retrieves the current densification method.
     * \return The current densification method.
     */
    virtual auto currentDensificationMethod() const -> QString = 0;

    /*!
     * \brief autoSegmentation Checks if auto-segmentation is enabled.
     * \return True if auto-segmentation is enabled, otherwise false.
     */
    virtual auto autoSegmentation() const -> bool = 0;

signals:

    /*!
     * \brief densificationChanged Signal emitted when the densification method changes.
     * \param[in] densification The new densification method.
     */
    void densificationChanged(QString densification);

    /*!
     * \brief run Signal emitted to start the densification process.
     */
    void run();

public slots:

    /*!
     * \brief Adds a densification widget to the view.
     * \param[in] densification The densification widget to add.
     */
    virtual void addDensification(QWidget *densification) = 0;

    /*!
     * \brief Sets the current densification method.
     * \param[in] densificationMethod The densification method to set.
     */
    virtual void setCurrentDensificationMethod(const QString &densificationMethod) = 0;


    virtual void setAutoSegmentation(bool autoSegmentation) = 0;

};

} // End namespace graphos

#endif // GRAPHOS_DENSE_VIEW_INTERFACE_H
