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

#ifndef GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"

class QAbstractItemModel;

namespace graphos
{

class GeoreferenceView
  : public DialogView
{

    Q_OBJECT

public:

    GeoreferenceView(QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags())
        : DialogView(parent, f)
    {
    }
    ~GeoreferenceView() override = default;

public:

    virtual auto crs() const -> QString = 0;

public slots:

    virtual void setProjectPath(const QString &path) = 0;
    virtual void setImageList(const std::vector<std::pair<size_t, QString>> &imageList) = 0;
    virtual void setCurrentImage(const QString &imagePath) = 0;
    virtual void setItemModelGroundControlPoints(QAbstractItemModel *model) = 0;
    virtual void setItemModelImagePoints(QAbstractItemModel *model) = 0;
    virtual void setEnableImagePointsAddOrEdit(bool active) = 0;
    virtual void setPoints(const std::list<std::pair<QString, QPointF>> &points) = 0;
    virtual void setCrs(const QString &crs) = 0;

private slots:

    virtual void removeGroundControlPoints() = 0;
    virtual void importGCP() = 0;
    virtual void exportGCP() = 0;

signals:

    void image_changed(size_t);
    void crsChange(QString);
    void addGroundControlPoint();
    void removeGroundControlPoint(int);
    void add_image_point(const QString &, size_t, const QPointF &);
    void remove_image_point(const QString &, size_t);
    void georeference();
    void export_gcp(const QString&/*file*/, const QString&/*format*/);
    void import_gcp(const QString&/*file*/, const QString&/*format*/);

};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H
