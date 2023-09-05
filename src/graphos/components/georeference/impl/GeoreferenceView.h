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

#ifndef GRAPHOS_GEOREFERENCE_VIEW_H
#define GRAPHOS_GEOREFERENCE_VIEW_H

#include "graphos/components/georeference/GeoreferenceView.h"

#include <tidop/geometry/entities/point.h>

#include <QItemSelection>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTableView;

namespace graphos
{

class GraphicViewer;

class GeoreferenceViewImp
    : public GeoreferenceView
{

    Q_OBJECT

public:

    GeoreferenceViewImp(QWidget *parent = nullptr,
                        Qt::WindowFlags f = Qt::WindowFlags());
    ~GeoreferenceViewImp() override;

protected slots:

    void openFile();
    void onGraphicsViewSelectionChanged();
    void onGraphicsViewRemoveSelectItems();

    void selectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
    //void onTreeWidgetItemSelectionChanged();
    void clickedPoint(const QPointF &point);

// GeoreferenceView interface

public:

    //QString orientationFile() const override;

public slots:

    void setProjectPath(const QString &path) override;
    void setImageList(const std::vector<std::pair<size_t, QString>> &imageList) override;
    void setCurrentImage(const QString &imagePath) override;
    void setItemModelGroundControlPoints(QAbstractItemModel *model) override;
    void setItemModelImagePoints(QAbstractItemModel *model) override;
    //void setTableHeader(const QStringList &header) override;
    void setEnableImagePointsAddOrEdit(bool active) override;
    void setPoints(const std::list<std::pair<QString, QPointF>> &points) override;
    void setCrs(const QString &crs) override;

private slots:

    void removeGroundControlPoints() override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

protected:

    QLabel *mLabelCRS;
    QLineEdit *mLineEditCRS;
    //QTreeWidget *mTreeWidgetGroundControlPoints;
    QTableView *mTableViewGroundControlPoints;
    QPushButton *mPushButtonAddPoint;
    QPushButton *mPushButtonDeletePoint;
    QPushButton *mPushButtonGeoreference;
    QSpacerItem *verticalSpacer;

    //QTreeWidget *mTreeWidgetImagePoints;
    QLabel *mLabelFilterByControlPoint;
    QComboBox *mComboBoxFilterByControlPoint;
    QLabel *mLabelFilterByImage;
    QComboBox *mComboBoxFilterByImage;
    QLabel *mLabelImagePoints;
    QTableView *mTableViewImagePoints;
    GraphicViewer *mGraphicViewerWidget;
    QLabel *mLabelImage;
    QComboBox *mComboBoxImages;
    QDialogButtonBox *mButtonBox;
    QString mProjectPath;

    bool bSelectedItem;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_VIEW_H
