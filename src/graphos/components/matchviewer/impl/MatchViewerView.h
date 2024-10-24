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

#ifndef GRAPHOS_MATCH_VIEWER_VIEW_H
#define GRAPHOS_MATCH_VIEWER_VIEW_H

#include "graphos/components/matchviewer/MatchViewerView.h"

class QDialogButtonBox;
class QTreeWidget;
class QComboBox;
class QTreeWidgetItem;
class QLabel;

namespace graphos
{

class GraphicViewer;

class MatchViewerViewImp
  : public MatchViewerView
{

    Q_OBJECT

public:

    MatchViewerViewImp(QWidget *parent = nullptr,
                       Qt::WindowFlags f = Qt::WindowFlags());
    ~MatchViewerViewImp() override;

protected slots:

    void onTreeWidgetMatchesItemClicked(QTreeWidgetItem *item, int col) const;
    void onTreeWidgetMatchesItemSelectionChanged();
    void onGraphicsViewLeftSelectionChanged();
    void onGraphicsViewRightSelectionChanged();

// MatchViewerView interface

public:

    void setLeftImage(const QString &imageLeft) override;
    void setRightImage(const QString &imageRight) override;
    void setLeftImageList(const std::vector<std::pair<size_t, QString>> &leftImageList) override;
    void setRightImageList(const std::vector<std::pair<size_t, QString>> &rightImageList) override;
    void setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> &matches) override;
    void setBackgroundColor(const QString &bgColor) override;
    void setSelectedMarkerStyle(const QString &color, int width) override;
    void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) override;
    void setLineStyle(const QString &color, int width) override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

// QWidget interface

protected:

    void closeEvent(QCloseEvent *event) override;

protected:

    QDialogButtonBox *mButtonBox;
    QTreeWidget *mTreeWidgetMatches;
    QLabel *mLabelLeftImage;
    QLabel *mLabelRightImage;
    QComboBox *mComboBoxLeftImage;
    QComboBox *mComboBoxRightImage;
    GraphicViewer *mGraphicsViewRightImage;
    GraphicViewer *mGraphicsViewLeftImage;
    QLabel *mLabelMatches;
    QPushButton *mPushButtonDeleteMatch;
    bool bUnsavedChanges;
    QString mMarkerColor;
    int mMarkerSize;
    int mMarkerWidth;
    int mMarkerType;
    QString mLineColor;
    int mLineWidth;
    QString mSelectedMarkerColor;
    int mSelectedMarkerWidth;
};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_VIEW_H
