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

#ifndef GRAPHOS_SETTINGS_MODEL_INTERFACE_H
#define GRAPHOS_SETTINGS_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{


class SettingsModel
  : public Model
{

    Q_OBJECT

public:

    SettingsModel(QObject *parent = nullptr) : Model(parent) {}
    ~SettingsModel() override = default;

    /*!
     * \brief List of available languages
     * \return
     */
    virtual auto languages() const -> QStringList = 0;

    /*!
     * \brief Current language
     * \return Current language
     */
    virtual auto language() const -> QString = 0;

    /*!
     * \brief Recent history projects
     * \return List of recent projects
     */
     //virtual QStringList history() const = 0;

     /*!
      * \brief Maximum history size
      * \return Maximum history size
      */
    virtual auto historyMaxSize() const -> int = 0;

    /*!
     * \brief Image viewer Background color
     * \return Hex value
     */
    virtual auto imageViewerBGcolor() const -> QString = 0;

    virtual auto useCuda() const -> bool = 0;
    virtual auto checkDevice() const -> bool = 0;
    ///TODO: Sift Properties

    virtual auto keypointsViewerBGColor() const -> QString = 0;
    virtual auto keypointsViewerMarkerType() const -> int = 0;
    virtual auto keypointsViewerMarkerSize() const -> int = 0;
    virtual auto keypointsViewerMarkerWidth() const -> int = 0;
    virtual auto keypointsViewerMarkerColor() const -> QString = 0;
    virtual auto keypointsViewerSelectMarkerWidth() const -> int = 0;
    virtual auto keypointsViewerSelectMarkerColor() const -> QString = 0;

    virtual auto matchesViewerBGColor() const -> QString = 0;
    virtual auto matchesViewerMarkerType() const -> int = 0;
    virtual auto matchesViewerMarkerSize() const -> int = 0;
    virtual auto matchesViewerMarkerWidth() const -> int = 0;
    virtual auto matchesViewerMarkerColor() const -> QString = 0;
    virtual auto matchesViewerSelectMarkerWidth() const -> int = 0;
    virtual auto matchesViewerSelectMarkerColor() const -> QString = 0;
    virtual auto matchesViewerLineColor() const -> QString = 0;
    virtual auto matchesViewerLineWidth() const -> int = 0;


    /*!
     * \brief read
     * \return
     */
    virtual void read() = 0;

    /*!
     * \brief write
     * \return
     */
    virtual void write() = 0;

public slots:

    /*!
     * \brief Set the language
     * \param[in] language GUI language
     */
    virtual void setLanguage(const QString &language) = 0;

    /*!
     * \brief Add a project to the history
     * \param[in] project Project path
     */
     //virtual void addToHistory(const QString &project) = 0;

     /*!
      * \brief Clear the history of recent projects
      */
      //virtual void clearHistory() = 0;

      /*!
       * \brief Set the size number of history items
       * \param[in] maxSize History size
       */
    virtual void setHistoryMaxSize(int maxSize) = 0;

    /*!
     * \brief Set Image Viewer background color
     * \param bgColor Hex value
     */
    virtual void setImageViewerBGcolor(const QString &bgColor) = 0;

    virtual void setUseCuda(bool active) = 0;

    virtual void setKeypointsViewerBGColor(const QString &color) = 0;
    virtual void setKeypointsViewerMarkerType(int type) = 0;
    virtual void setKeypointsViewerMarkerSize(int size) = 0;
    virtual void setKeypointsViewerMarkerWidth(int width) = 0;
    virtual void setKeypointsViewerMarkerColor(const QString &color) = 0;
    virtual void setKeypointsViewerSelectMarkerWidth(int width) = 0;
    virtual void setKeypointsViewerSelectMarkerColor(const QString &color) = 0;


    virtual void setMatchesViewerBGColor(const QString &color) = 0;
    virtual void setMatchesViewerMarkerType(int type) = 0;
    virtual void setMatchesViewerMarkerSize(int size) = 0;
    virtual void setMatchesViewerMarkerWidth(int width) = 0;
    virtual void setMatchesViewerMarkerColor(const QString &color) = 0;
    virtual void setMatchesViewerSelectMarkerWidth(int width) = 0;
    virtual void setMatchesViewerSelectMarkerColor(const QString &color) = 0;
    virtual void setMatchesViewerLineColor(const QString &color) = 0;
    virtual void setMatchesViewerLineWidth(int width) = 0;

signals:

    void unsavedChanges(bool);
};


} // namespace graphos

#endif // GRAPHOS_SETTINGS_MODEL_INTERFACE_H
