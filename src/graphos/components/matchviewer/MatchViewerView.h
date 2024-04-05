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

#ifndef GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{

class MatchViewerView
  : public DialogView
{
    Q_OBJECT

public:

    explicit MatchViewerView(QWidget *parent = nullptr,
                             Qt::WindowFlags f = Qt::WindowFlags())
        : DialogView(parent, f)
    {
    }

    ~MatchViewerView() override = default;

    /*!
     * \brief Set the left image
     * \param[in] imageLeft Left image
     */
    virtual void setLeftImage(const QString &imageLeft) = 0;

    /*!
     * \brief Set the right image
     * \param[in] imageRight Right image
     */
    virtual void setRightImage(const QString &imageRight) = 0;

    /*!
     * \brief Set the list of images for image selector left
     * \param[in] leftImageList List of left images
     */
    virtual void setLeftImageList(const std::vector<std::pair<size_t, QString>> &leftImageList) = 0;

    /*!
     * \brief Set the list of images for image selector right
     * \param[in] rightImageList List of right images
     */
    virtual void setRightImageList(const std::vector<std::pair<size_t, QString>> &rightImageList) = 0;

    virtual void setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> &matches) = 0;

    virtual void setBackgroundColor(const QString &bgColor) = 0;
    virtual void setSelectedMarkerStyle(const QString &color, int width) = 0;
    virtual void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) = 0;
    virtual void setLineStyle(const QString &color, int width) = 0;

signals:

    void leftImageChange(size_t);
    void rightImageChange(size_t);
    void loadMatches(size_t, size_t);

};

} // namespace graphos


#endif // GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H
