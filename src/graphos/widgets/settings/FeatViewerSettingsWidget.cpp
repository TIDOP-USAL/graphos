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

#include "FeatViewerSettingsWidget.h"

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

FeatureViewerSettingsWidget::FeatureViewerSettingsWidget(QWidget *parent)
    : GraphosWidgetView(parent)
{
    initUI();
    initSignalAndSlots();
}

QString FeatureViewerSettingsWidget::backgroundColor() const
{
    return mLineEditBackgroundColor->text();
}

int FeatureViewerSettingsWidget::markerType() const
{
    return mListWidgetMarkerType->currentRow();
}

int FeatureViewerSettingsWidget::markerSize() const
{
    return mSpinBoxMarkerSize->value();
}

int FeatureViewerSettingsWidget::markerWidth() const
{
    return mSpinBoxMarkerWidth->value();
}

QString FeatureViewerSettingsWidget::markerColor() const
{
    return mLineEditMarkerColor->text();
}

int FeatureViewerSettingsWidget::selectedMarkerWidth() const
{
    return mSpinBoxSelectMarkerWidth->value();
}

QString FeatureViewerSettingsWidget::selectedMarkerColor() const
{
    return mLineEditSelectMarkerColor->text();
}

void FeatureViewerSettingsWidget::setBackgroundColor(const QString &color)
{
    const QSignalBlocker blockerKeypointViewerBGColor(mLineEditBackgroundColor);
    mLineEditBackgroundColor->setText(color);
}

void FeatureViewerSettingsWidget::setMarkerType(int type)
{
    const QSignalBlocker blockerKeypointsMarker(mListWidgetMarkerType);
    mListWidgetMarkerType->setCurrentRow(type);
}

void FeatureViewerSettingsWidget::setMarkerSize(int size)
{
    const QSignalBlocker blocker(mSpinBoxMarkerSize);
    mSpinBoxMarkerSize->setValue(size);
}

void FeatureViewerSettingsWidget::setMarkerWidth(int width)
{
    const QSignalBlocker blockerKeypointViewerWidth(mSpinBoxMarkerWidth);
    mSpinBoxMarkerWidth->setValue(width);
}

void FeatureViewerSettingsWidget::setMarkerColor(const QString &color)
{
    const QSignalBlocker blockerKeypointMarkerColor(mLineEditMarkerColor);
    mLineEditMarkerColor->setText(color);
}

void FeatureViewerSettingsWidget::setSelectedMarkerWidth(int width)
{
    const QSignalBlocker blockerSelectKeypointViewerWidth(mSpinBoxSelectMarkerWidth);
    mSpinBoxSelectMarkerWidth->setValue(width);
}

void FeatureViewerSettingsWidget::setSelectedMarkerColor(const QString &color)
{
    const QSignalBlocker blockerSelectKeypointViewerColor(mLineEditSelectMarkerColor);
    mLineEditSelectMarkerColor->setText(color);
}

void FeatureViewerSettingsWidget::onPushButtonBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditBackgroundColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditBackgroundColor->setText(color.name());
}

void FeatureViewerSettingsWidget::onPushButtonMarkerColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditMarkerColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditMarkerColor->setText(color.name());
}

void FeatureViewerSettingsWidget::onPushButtonSelectedMarkerColorClicked()
{
    QColor color = QColorDialog::getColor(QColor(mLineEditSelectMarkerColor->text()), this, "Pick a color", QColorDialog::DontUseNativeDialog);
    if (color.isValid())
        mLineEditSelectMarkerColor->setText(color.name());
}

void FeatureViewerSettingsWidget::update()
{
    mSpinBoxMarkerSize->setDisabled(mListWidgetMarkerType->currentRow() == 0);
}

void FeatureViewerSettingsWidget::retranslate()
{
    setWindowTitle(QApplication::translate("FeatureViewerSettingsWidget", "Feature viewer", nullptr));
    mLabelBackgroundColor->setText(QApplication::translate("FeatureViewerSettingsWidget", "Background Color:", nullptr));
    mGroupBoxMVMarker->setTitle(QApplication::translate("FeatureViewerSettingsWidget", "Marker", nullptr));
    mLabelMarkerSize->setText(QApplication::translate("FeatureViewerSettingsWidget", "Size:", nullptr));
    mLabelMarkerWidth->setText(QApplication::translate("FeatureViewerSettingsWidget", "Width:", nullptr));
    mLabelMarkerColor->setText(QApplication::translate("FeatureViewerSettingsWidget", "Color:", nullptr));
    mGroupBoxSelectMarker->setTitle(QApplication::translate("FeatureViewerSettingsWidget", "Selected marker"));
    mLabelSelectMarkerWidth->setText(QApplication::translate("MatchViewerSettingsWidget", "Width:", nullptr));
    mLabelSelectMarkerColor->setText(QApplication::translate("MatchViewerSettingsWidget", "Color:", nullptr));

}

void FeatureViewerSettingsWidget::clear()
{
    const QSignalBlocker blockerKeypointViewerBGColor(mLineEditBackgroundColor);
    const QSignalBlocker blockerKeypointsMarker(mListWidgetMarkerType);
    const QSignalBlocker blockerKeypointMarkerSize(mSpinBoxMarkerSize);
    const QSignalBlocker blockerKeypointViewerWidth(mSpinBoxMarkerWidth);
    const QSignalBlocker blockerKeypointMarkerColor(mLineEditMarkerColor);
    const QSignalBlocker blockerSelectKeypointViewerWidth(mSpinBoxSelectMarkerWidth);
    const QSignalBlocker blockerSelectKeypointViewerColor(mLineEditSelectMarkerColor);

    mLineEditBackgroundColor->setText("#dcdcdc");
    mListWidgetMarkerType->setCurrentRow(0);
    mSpinBoxMarkerSize->setValue(20);
    mSpinBoxMarkerWidth->setValue(2);
    mLineEditMarkerColor->setText("#00aa00");
    mSpinBoxSelectMarkerWidth->setValue(2);
    mLineEditSelectMarkerColor->setText("#e5097e");
}

void FeatureViewerSettingsWidget::initUI()
{

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollKeypointsViewer = new QScrollArea(this);
    scrollKeypointsViewer->setWidgetResizable(true);
    scrollKeypointsViewer->setFrameShape(QFrame::Shape::NoFrame);
    QWidget *scrollAreaWidgetContentsViewerKeypoints = new QWidget(this);

    QGridLayout *gridLayoutContentsFeaturesViewer = new QGridLayout(scrollAreaWidgetContentsViewerKeypoints);

    mLabelBackgroundColor = new QLabel(this);
    gridLayoutContentsFeaturesViewer->addWidget(mLabelBackgroundColor, 0, 0, 1, 1);
    mLineEditBackgroundColor = new QLineEdit(this);
    mLineEditBackgroundColor->setText("#dcdcdc");
    gridLayoutContentsFeaturesViewer->addWidget(mLineEditBackgroundColor, 0, 1, 1, 1);
    mPushButtonBackgroundColor = new QPushButton(tr("..."), this);
    mPushButtonBackgroundColor->setMaximumSize(QSize(23, 23));
    gridLayoutContentsFeaturesViewer->addWidget(mPushButtonBackgroundColor, 0, 2, 1, 1);

    /* Marker */

    mGroupBoxMVMarker = new QGroupBox(tr("Marker"), this);
    gridLayoutContentsFeaturesViewer->addWidget(mGroupBoxMVMarker, 1, 0, 1, 3);
    QGridLayout *layoutKPVMarker = new QGridLayout();
    mGroupBoxMVMarker->setLayout(layoutKPVMarker);

    mLabelMarkerType = new QLabel(this);
    layoutKPVMarker->addWidget(mLabelMarkerType, 0, 0, 1, 1);
    mListWidgetMarkerType = new QListWidget(this);
    mListWidgetMarkerType->setIconSize(QSize(30, 30));
    mListWidgetMarkerType->setViewMode(QListWidget::ListMode);
    mListWidgetMarkerType->setResizeMode(QListWidget::Fixed);
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint.png"), "Scale/Distance", mListWidgetMarkerType));
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_circle.png"), "Circle", mListWidgetMarkerType));
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_cross.png"), "Cross", mListWidgetMarkerType));
    mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_diag_cross.png"), "Diagonal cross", mListWidgetMarkerType));
    mListWidgetMarkerType->setCurrentRow(0);
    layoutKPVMarker->addWidget(mListWidgetMarkerType, 0, 1, 1, 2);

    mLabelMarkerSize = new QLabel(this);
    layoutKPVMarker->addWidget(mLabelMarkerSize, 1, 0, 1, 2);
    mSpinBoxMarkerSize = new QSpinBox(this);
    mSpinBoxMarkerSize->setRange(0, 100);
    mSpinBoxMarkerSize->setValue(20);
    layoutKPVMarker->addWidget(mSpinBoxMarkerSize, 1, 1, 1, 2);

    mLabelMarkerWidth = new QLabel(this);
    layoutKPVMarker->addWidget(mLabelMarkerWidth, 2, 0, 1, 1);
    mSpinBoxMarkerWidth = new QSpinBox(this);
    mSpinBoxMarkerWidth->setRange(0, 50);
    mSpinBoxMarkerWidth->setValue(2);
    layoutKPVMarker->addWidget(mSpinBoxMarkerWidth, 2, 1, 1, 2);

    mLabelMarkerColor = new QLabel(this);
    layoutKPVMarker->addWidget(mLabelMarkerColor, 3, 0, 1, 1);
    mLineEditMarkerColor = new QLineEdit(this);
    mLineEditMarkerColor->setText("#e5097e");
    layoutKPVMarker->addWidget(mLineEditMarkerColor, 3, 1, 1, 1);
    mPushButtonMarkerColor = new QPushButton(tr("..."), this);
    mPushButtonMarkerColor->setMaximumSize(QSize(23, 23));
    layoutKPVMarker->addWidget(mPushButtonMarkerColor, 3, 2, 1, 1);

    /* Select marker */

    mGroupBoxSelectMarker = new QGroupBox(this);
    gridLayoutContentsFeaturesViewer->addWidget(mGroupBoxSelectMarker, 2, 0, 1, 3);
    QGridLayout *layoutKPVSelectMarker = new QGridLayout();
    mGroupBoxSelectMarker->setLayout(layoutKPVSelectMarker);

    mLabelSelectMarkerWidth = new QLabel(this);
    layoutKPVSelectMarker->addWidget(mLabelSelectMarkerWidth, 0, 0, 1, 1);
    mSpinBoxSelectMarkerWidth = new QSpinBox(this);
    mSpinBoxSelectMarkerWidth->setRange(0, 50);
    mSpinBoxSelectMarkerWidth->setValue(2);
    layoutKPVSelectMarker->addWidget(mSpinBoxSelectMarkerWidth, 0, 1, 1, 2);

    mLabelSelectMarkerColor = new QLabel(this);
    layoutKPVSelectMarker->addWidget(mLabelSelectMarkerColor, 1, 0, 1, 1);
    mLineEditSelectMarkerColor = new QLineEdit(this);
    mLineEditSelectMarkerColor->setText("#ff0000");
    layoutKPVSelectMarker->addWidget(mLineEditSelectMarkerColor, 1, 1, 1, 1);
    mPushButtonSelectMarkerColor = new QPushButton(tr("..."), this);
    mPushButtonSelectMarkerColor->setMaximumSize(QSize(23, 23));
    layoutKPVSelectMarker->addWidget(mPushButtonSelectMarkerColor, 1, 2, 1, 1);

    gridLayoutContentsFeaturesViewer->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding), 3, 1, 1, 1);

    scrollKeypointsViewer->setWidget(scrollAreaWidgetContentsViewerKeypoints);
    layout->addWidget(scrollKeypointsViewer);

    retranslate();
    clear();
    update();
}

void FeatureViewerSettingsWidget::initSignalAndSlots()
{
    connect(mLineEditBackgroundColor, SIGNAL(textChanged(QString)), this, SIGNAL(backgroundColorChange(QString)));
    connect(mPushButtonBackgroundColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonBackgroundColorClicked()));
    connect(mSpinBoxMarkerSize, SIGNAL(valueChanged(int)), this, SIGNAL(markerSizeChange(int)));
    connect(mSpinBoxMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(markerWidthChange(int)));
    connect(mLineEditMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(markerColorChange(QString)));
    connect(mPushButtonMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMarkerColorClicked()));
    connect(mSpinBoxSelectMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(selectedMarkerWidthChange(int)));
    connect(mLineEditSelectMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(selectedMarkerColorChange(QString)));
    connect(mPushButtonSelectMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonSelectedMarkerColorClicked()));
    connect(mListWidgetMarkerType, SIGNAL(currentRowChanged(int)), this, SIGNAL(markerTypeChange(int)));
    connect(mListWidgetMarkerType, SIGNAL(currentTextChanged(QString)), this, SLOT(update()));
}


} // namespace graphos
