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

#ifndef GRAPHOS_PROGRESS_H
#define GRAPHOS_PROGRESS_H

#include <QObject>

#include <tidop/core/progress.h>

namespace graphos
{

/*!
 * \brief Progress handler
 *
 * The `ProgressHandler` class provides functionality for handling progress updates during a task execution.
 * It inherits from QObject and tl::ProgressBase, enabling it to emit signals and handle progress-related operations.
 */
class ProgressHandler
  : public QObject,
    public tl::ProgressBase
{
    Q_OBJECT

public:

    /*!
     * \brief Default constructor
     *
     * Constructs a `ProgressHandler` object with no parent.
     *
     * \param[in] parent The parent object (optional).
     */
    ProgressHandler(QObject *parent = nullptr);

    /*!
     * \brief Constructor with range initialization
     *
     * Constructs a `ProgressHandler` object with the specified minimum and maximum values for the progress range.
     *
     * \param[in] min The minimum value of the progress range.
     * \param[in] max The maximum value of the progress range.
     * \param[in] parent The parent object (optional).
     */
    ProgressHandler(size_t min, size_t max, QObject *parent = nullptr);
    ~ProgressHandler() override;

signals:

    /*!
     * \brief Signal emitted when the range of the progress changes
     *
     * \param[in] min The new minimum value of the progress range.
     * \param[in] max The new maximum value of the progress range.
     */
    void rangeChange(int, int);

    /*!
     * \brief Signal emitted when the value of the progress changes
     *
     * \param[in] value The new value of the progress.
     */
    void valueChange(int);

    /*!
     * \brief Signal emitted when the progress is initialized
     */
    void initialized();

    /*!
     * \brief Signal emitted when the progress is finished
     */
    void finished();

    /*!
     * \brief Signal emitted when the title of the progress changes
     *
     * \param title The new title of the progress.
     */
    void titleChange(QString);

    /*!
     * \brief Signal emitted when the description of the progress changes
     *
     * \param description The new description of the progress.
     */
    void descriptionChange(QString);

    /*!
     * \brief Signal emitted when the progress is canceled
     */
    void cancel();

    /*!
     * \brief Signal emitted to control whether the progress should be automatically closed
     *
     * \param active True if automatic closing is enabled, false otherwise.
     */
    void closeAuto(bool);

public slots:

    /*!
     * \brief Slot to initialize the progress
     */
    void init();

    /*!
     * \brief Slot to finish the progress
     */
    void finish();

    /*!
     * \brief Slot to set the title of the progress
     *
     * \param[in] title The new title of the progress.
     */
    void setTitle(const QString &title);

    /*!
     * \brief Slot to set the description of the progress
     *
     * \param[in] description The new description of the progress.
     */
    void setDescription(const QString &description);

    /*!
     * \brief Slot to control whether the progress should be automatically closed
     *
     * \param[in] active True if automatic closing is enabled, false otherwise.
     */
    void setCloseAuto(bool active);

// ProgressBase interface

public:

    /*!
     * \brief Set the range of the progress
     *
     * Overrides the base class method to set the range of the progress.
     *
     * \param[in] min The minimum value of the progress range.
     * \param[in] max The maximum value of the progress range.
     */
    void setRange(size_t min, size_t max) override;

protected:

    void updateProgress() override;
    void terminate() override;

};


} // namespace graphos

#endif // GRAPHOS_PROGRESS_H
