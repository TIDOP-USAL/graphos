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

#include "MatchViewerSettingsWidget.h"

TL_DISABLE_WARNINGS
#include <QSpinBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QColorDialog>
#include <QScrollArea>
#include <QPushButton>
TL_DEFAULT_WARNINGS

namespace graphos
{

MatchViewerSettingsWidget::MatchViewerSettingsWidget(QWidget *parent)
    : GraphosWidgetView(parent)
{
    MatchViewerSettingsWidget::initUI();
    MatchViewerSettingsWidget::initSignalAndSlots();
}

auto MatchViewerSettingsWidget::backgroundColor() const -> QString
{
    return mLineEditBackgroundColor->text();
}

auto MatchViewerSettingsWidget::markerType() const -> int
{
    return mListWidgetMarkerType->currentRow();
}

auto MatchViewerSettingsWidget::markerSize() const -> int
{
    return mSpinBoxMarkerSize->value();
}

auto MatchViewerSettingsWidget::markerWidth() const -> int
{
    return mSpinBoxMarkerWidth->value();
}

auto MatchViewerSettingsWidget::markerColor() const -> QString
{
    return mLineEditMarkerColor->text();
}

auto MatchViewerSettingsWidget::selectedMarkerWidth() const -> int
{
    return mSpinBoxSelectMarkerWidth->value();
}

auto MatchViewerSettingsWidget::selectedMarkerColor() const -> QString
{
    return mLineEditSelectMarkerColor->text();
}

auto MatchViewerSettingsWidget::lineColor() const -> QString
{
    return mLineEditLineColor->text();
}

auto MatchViewerSettingsWidget::lineWidth() const -> int
{
    return mSpinBoxLineWidth->value();
}

void MatchViewerSettingsWidget::setBackgroundColor(const QString &color)
{
    const QSignalBlocker blockerMatchesViewerBGColo(mLineEditBackgroundColor);
    mLineEditBackgroundColor->setText(color);
}

void MatchViewerSettingsWidget::setMarkerType(int type)
{
    const QSignalBlocker blockerMatchesMarker(mListWidgetMarkerType);
    mListWidgetMarkerType->setCurrentRow(type);
}

void MatchViewerSettingsWidget::setMarkerSize(int size)
{
    const QSignalBlocker blocker(mSpinBoxMarkerSize);
    mSpinBoxMarkerSize->setValue(size);
}

void MatchViewerSettingsWidget::setMarkerWidth(int width)
{
    const QSignalBlocker blockerMatchesViewerMarkerWidth(mSpinBoxMarkerWidth);
    mSpinBoxMarkerWidth->setValue(width);
}

void MatchViewerSettingsWidget::setMarkerColor(const QString &color)
{
    const QSignalBlocker blockerMatchesMarkerColor(mLineEditMarkerColor);
    mLineEditMarkerColor->setText(color);
}

void MatchViewerSettingsWidget::setSelectedMarkerWidth(int width)
{
    const QSignalBlocker blockerMatchesSelectMarkerColor(mSpinBoxSelectMarkerWidth);
    mSpinBoxSelectMarkerWidth->setValue(width);
}

void MatchViewerSettingsWidget::setSelectedMarkerColor(const QString &color)
{
    const QSignalBlocker blockerSelectMatchesMarkerColor(mLineEditSelectMarkerColor);
    mLineEditSelectMarkerColor->setText(color);
}

void MatchViewerSettingsWidget::setLineColor(const QString &color)
{
    const QSignalBlocker blockerMatchesViewerLineColor(mLineEditLineColor);
    mLineEditLineColor->setText(color);
}

void MatchViewerSettingsWidget::setLineWidth(int width)
{
    const QSignalBlocker blocker(mSpinBoxLineWidth);
    mSpinBoxLineWidth->setValue(width);
}

void MatchViewerSettingsWidget::onPushButtonBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditBackgroundColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditBackgroundColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMarkerColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditMarkerColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonSelectMarkerColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditSelectMarkerColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditSelectMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMatchViewerLineColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditLineColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditLineColor->setText(color.name());
}

void MatchViewerSettingsWidget::update()
{
    mSpinBoxMarkerSize->setDisabled(mListWidgetMarkerType->currentRow() == 0);
}

void MatchViewerSettingsWidget::retranslate()
{
    setWindowTitle(QApplication::translate("MatchViewerSettingsWidget", "Match viewer", nullptr));
    mLabelBackgroundColor->setText(QApplication::translate("MatchViewerSettingsWidget", "Background Color:", nullptr));
    mGroupBoxMVMarker->setTitle(QApplication::translate("MatchViewerSettingsWidget", "Marker", nullptr));
    mLabelMarkerType->setText(QApplication::translate("MatchViewerSettingsWidget", "Type:", nullptr));
    mLabelMarkerSize->setText(QApplication::translate("MatchViewerSettingsWidget", "Size:", nullptr));
    mLabelMarkerWidth->setText(QApplication::translate("MatchViewerSettingsWidget", "Width:", nullptr));
    mLabelMarkerColor->setText(QApplication::translate("MatchViewerSettingsWidget", "Color:", nullptr));
    mGroupBoxSelectMatches->setTitle(QApplication::translate("MatchViewerSettingsWidget", "Select matches", nullptr));
    mLabelSelectMarkerWidth->setText(QApplication::translate("MatchViewerSettingsWidget", "Width:", nullptr));
    mLabelSelectMarkerColor->setText(QApplication::translate("MatchViewerSettingsWidget", "Color:", nullptr));
    mGroupBoxLine->setTitle(QApplication::translate("MatchViewerSettingsWidget", "Line", nullptr));
    mLabelLineWidth->setText(QApplication::translate("MatchViewerSettingsWidget", "Width:", nullptr));
    mLabelLineColor->setText(QApplication::translate("MatchViewerSettingsWidget", "Color:", nullptr));
}

void MatchViewerSettingsWidget::clear()
{
    const QSignalBlocker blocker_keypoint_viewer_bg_color(mLineEditBackgroundColor);
    const QSignalBlocker blocker_keypoints_marker(mListWidgetMarkerType);
    const QSignalBlocker blocker_keypoint_marker_size(mSpinBoxMarkerSize);
    const QSignalBlocker blocker_keypoint_viewer_width(mSpinBoxMarkerWidth);
    const QSignalBlocker blocker_keypoint_marker_color(mLineEditMarkerColor);
    const QSignalBlocker blocker_select_keypoint_viewer_width(mSpinBoxSelectMarkerWidth);
    const QSignalBlocker blocker_select_keypoint_viewer_color(mLineEditSelectMarkerColor);

    mLineEditBackgroundColor->setText("#dcdcdc");
    mListWidgetMarkerType->setCurrentRow(0);
    mSpinBoxMarkerSize->setValue(20);
    mSpinBoxMarkerWidth->setValue(2);
    mLineEditMarkerColor->setText("#00aa00");
    mSpinBoxSelectMarkerWidth->setValue(2);
    mLineEditSelectMarkerColor->setText("#e5097e");
}

void MatchViewerSettingsWidget::initUI()
{

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scroll_matches_viewer = new QScrollArea(this);
    scroll_matches_viewer->setWidgetResizable(true);
    scroll_matches_viewer->setFrameShape(QFrame::Shape::NoFrame);
    QWidget *scroll_area_widget_contents_viewer_matches = new QWidget(this);

    QGridLayout *grid_layout_contents_matches_viewer = new QGridLayout(scroll_area_widget_contents_viewer_matches);

    mLabelBackgroundColor = new QLabel(this);
    grid_layout_contents_matches_viewer->addWidget(mLabelBackgroundColor, 0, 0, 1, 1);
    mLineEditBackgroundColor = new QLineEdit(this);
    mLineEditBackgroundColor->setText("#dcdcdc");
    grid_layout_contents_matches_viewer->addWidget(mLineEditBackgroundColor, 0, 1, 1, 1);
    mPushButtonBackgroundColor = new QPushButton(tr("..."), this);
    mPushButtonBackgroundColor->setMaximumSize(QSize(23, 23));
    grid_layout_contents_matches_viewer->addWidget(mPushButtonBackgroundColor, 0, 2, 1, 1);

    /* Marker */

    mGroupBoxMVMarker = new QGroupBox(this);
    grid_layout_contents_matches_viewer->addWidget(mGroupBoxMVMarker, 1, 0, 1, 3);
    QGridLayout *layout_mv_marker = new QGridLayout();
    mGroupBoxMVMarker->setLayout(layout_mv_marker);

    mLabelMarkerType = new QLabel(this);
    layout_mv_marker->addWidget(mLabelMarkerType, 0, 0, 1, 1);
    mListWidgetMarkerType = new QListWidget(this);
    mListWidgetMarkerType->setIconSize(QSize(30, 30));
    mListWidgetMarkerType->setViewMode(QListWidget::ListMode);
    mListWidgetMarkerType->setResizeMode(QListWidget::Fixed);
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_circle.png"), "Circle", mListWidgetMarkerType));
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_cross.png"), "Cross", mListWidgetMarkerType));
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_diag_cross.png"), "Diagonal cross", mListWidgetMarkerType));
    mListWidgetMarkerType->setCurrentRow(0);
    layout_mv_marker->addWidget(mListWidgetMarkerType, 0, 1, 1, 2);

    mLabelMarkerSize = new QLabel(this);
    layout_mv_marker->addWidget(mLabelMarkerSize, 1, 0, 1, 2);
    mSpinBoxMarkerSize = new QSpinBox(this);
    mSpinBoxMarkerSize->setRange(0, 100);
    mSpinBoxMarkerSize->setValue(20);
    layout_mv_marker->addWidget(mSpinBoxMarkerSize, 1, 1, 1, 2);

    mLabelMarkerWidth = new QLabel(this);
    layout_mv_marker->addWidget(mLabelMarkerWidth, 2, 0, 1, 1);
    mSpinBoxMarkerWidth = new QSpinBox(this);
    mSpinBoxMarkerWidth->setRange(0, 50);
    mSpinBoxMarkerWidth->setValue(2);
    layout_mv_marker->addWidget(mSpinBoxMarkerWidth, 2, 1, 1, 2);

    mLabelMarkerColor = new QLabel(this);
    layout_mv_marker->addWidget(mLabelMarkerColor, 3, 0, 1, 1);
    mLineEditMarkerColor = new QLineEdit(this);
    mLineEditMarkerColor->setText("#e5097e");
    layout_mv_marker->addWidget(mLineEditMarkerColor, 3, 1, 1, 1);
    mPushButtonMarkerColor = new QPushButton(tr("..."), this);
    mPushButtonMarkerColor->setMaximumSize(QSize(23, 23));
    layout_mv_marker->addWidget(mPushButtonMarkerColor, 3, 2, 1, 1);

    /* Select matches */

    mGroupBoxSelectMatches = new QGroupBox(this);
    grid_layout_contents_matches_viewer->addWidget(mGroupBoxSelectMatches, 2, 0, 1, 3);
    QGridLayout *layout_mv_select_marker = new QGridLayout();
    mGroupBoxSelectMatches->setLayout(layout_mv_select_marker);

    mLabelSelectMarkerWidth = new QLabel(this);
    layout_mv_select_marker->addWidget(mLabelSelectMarkerWidth, 0, 0, 1, 1);
    mSpinBoxSelectMarkerWidth = new QSpinBox(this);
    mSpinBoxSelectMarkerWidth->setRange(0, 50);
    mSpinBoxSelectMarkerWidth->setValue(2);
    layout_mv_select_marker->addWidget(mSpinBoxSelectMarkerWidth, 0, 1, 1, 2);

    mLabelSelectMarkerColor = new QLabel(this);
    layout_mv_select_marker->addWidget(mLabelSelectMarkerColor, 1, 0, 1, 1);
    mLineEditSelectMarkerColor = new QLineEdit(this);
    mLineEditSelectMarkerColor->setText("#ff0000");
    layout_mv_select_marker->addWidget(mLineEditSelectMarkerColor, 1, 1, 1, 1);
    mPushButtonSelectMarkerColor = new QPushButton(tr("..."), this);
    mPushButtonSelectMarkerColor->setMaximumSize(QSize(23, 23));
    layout_mv_select_marker->addWidget(mPushButtonSelectMarkerColor, 1, 2, 1, 1);

    /* Line */

    mGroupBoxLine = new QGroupBox(this);
    grid_layout_contents_matches_viewer->addWidget(mGroupBoxLine, 3, 0, 1, 3);
    QGridLayout *layout_mv_line = new QGridLayout();
    mGroupBoxLine->setLayout(layout_mv_line);

    mLabelLineWidth = new QLabel(this);
    layout_mv_line->addWidget(mLabelLineWidth, 0, 0, 1, 1);
    mSpinBoxLineWidth = new QSpinBox(this);
    mSpinBoxLineWidth->setRange(0, 50);
    mSpinBoxLineWidth->setValue(2);
    layout_mv_line->addWidget(mSpinBoxLineWidth, 0, 1, 1, 2);

    mLabelLineColor = new QLabel(this);
    layout_mv_line->addWidget(mLabelLineColor, 1, 0, 1, 1);
    mLineEditLineColor = new QLineEdit(this);
    mLineEditLineColor->setText("#e5097e");
    layout_mv_line->addWidget(mLineEditLineColor, 1, 1, 1, 1);
    mPushButtonLineColor = new QPushButton(tr("..."), this);
    mPushButtonLineColor->setMaximumSize(QSize(23, 23));
    layout_mv_line->addWidget(mPushButtonLineColor, 1, 2, 1, 1);

    grid_layout_contents_matches_viewer->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding), 4, 1, 1, 1);

    scroll_matches_viewer->setWidget(scroll_area_widget_contents_viewer_matches);
    layout->addWidget(scroll_matches_viewer);

    retranslate();
    clear();
    update();
}

void MatchViewerSettingsWidget::initSignalAndSlots()
{
    connect(mLineEditBackgroundColor, SIGNAL(textChanged(QString)), this, SIGNAL(backgroundColorChange(QString)));
    connect(mPushButtonBackgroundColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonBackgroundColorClicked()));
    connect(mSpinBoxMarkerSize, SIGNAL(valueChanged(int)), this, SIGNAL(markerSizeChange(int)));
    connect(mSpinBoxMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(markerWidthChange(int)));
    connect(mLineEditMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(markerColorChange(QString)));
    connect(mPushButtonMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMarkerColorClicked()));
    connect(mSpinBoxSelectMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(selectedMarkerWidthChange(int)));
    connect(mLineEditSelectMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(selectedMarkerColorChange(QString)));
    connect(mPushButtonSelectMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonSelectMarkerColorClicked()));
    connect(mListWidgetMarkerType, SIGNAL(currentRowChanged(int)), this, SIGNAL(markerTypeChange(int)));
    connect(mListWidgetMarkerType, SIGNAL(currentTextChanged(QString)), this, SLOT(update()));
}


} // namespace graphos
