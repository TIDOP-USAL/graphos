/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H
#define GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

class QAbstractItemModel;

namespace graphos
{

class GeoreferenceView
  : public DialogView
{

  Q_OBJECT

public:

  GeoreferenceView(QWidget *parent,
                   Qt::WindowFlags f = Qt::WindowFlags()) 
    : DialogView(parent, f) {}
  ~GeoreferenceView() override = default;

  //virtual QString orientationFile() const = 0;

public slots:

  virtual void setProjectPath(const QString &path) = 0;
  virtual void setImageList(const std::vector<QString> &imageList)  = 0;
  virtual void setCurrentImage(const QString &image)  = 0;
  virtual void setItemModelGroundControlPoints(QAbstractItemModel *model) = 0;
  virtual void setItemModelImagePoints(QAbstractItemModel *model) = 0;
  //virtual void setTableHeader(const QStringList &header) = 0;
  virtual void setEnableImagePointsAddOrEdit(bool active) = 0;
  virtual void setPoints(const std::list<std::pair<QString, QPointF>> &points) = 0;
  virtual void setCrs(const QString &crs) = 0;

private slots: 

  virtual void removeGroundControlPoints() = 0;

signals:

  void imageChange(QString);
  void crsChange(QString);
  //void loadCSV(const QString &, const QString &);
  void addGroundControlPoint();
  void removeGroundControlPoint(int);
  void addImagePoint(const QString &, const QString &, const QPointF &);
  void removeImagePoint(const QString &, const QString &);
  void georeference();
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_VIEW_INTERFACE_H
