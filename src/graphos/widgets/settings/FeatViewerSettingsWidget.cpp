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
  return mLineEditKeypointViewerBGColor->text();
}

int FeatureViewerSettingsWidget::markerType() const
{
  return mListWidgetKeypointsViewerMarkerType->currentRow();
}

int FeatureViewerSettingsWidget::markerSize() const
{
  return mSpinBoxKeypointViewerMarkerSize->value();
}

int FeatureViewerSettingsWidget::markerWidth() const
{
  return mSpinBoxKeypointViewerMarkerWidth->value();
}

QString FeatureViewerSettingsWidget::markerColor() const
{
  return mLineEditKeypointViewerMarkerColor->text();
}

int FeatureViewerSettingsWidget::selectedMarkerWidth() const
{
  return mSpinBoxSelectKeypointViewerMarkerWidth->value();
}

QString FeatureViewerSettingsWidget::selectedMarkerColor() const
{
  return mLineEditSelectKeypointViewerMarkerColor->text();
}

void FeatureViewerSettingsWidget::setBackgroundColor(const QString &color)
{
  const QSignalBlocker blockerKeypointViewerBGColor(mLineEditKeypointViewerBGColor);
  mLineEditKeypointViewerBGColor->setText(color);
}

void FeatureViewerSettingsWidget::setMarkerType(int type)
{
  const QSignalBlocker blockerKeypointsMarker(mListWidgetKeypointsViewerMarkerType);
  mListWidgetKeypointsViewerMarkerType->setCurrentRow(type);
}

void FeatureViewerSettingsWidget::setMarkerSize(int size)
{
  const QSignalBlocker blocker(mSpinBoxKeypointViewerMarkerSize);
  mSpinBoxKeypointViewerMarkerSize->setValue(size);
}

void FeatureViewerSettingsWidget::setMarkerWidth(int width)
{
  const QSignalBlocker blockerKeypointViewerWidth(mSpinBoxKeypointViewerMarkerWidth);
  mSpinBoxKeypointViewerMarkerWidth->setValue(width);
}

void FeatureViewerSettingsWidget::setMarkerColor(const QString &color)
{
  const QSignalBlocker blockerKeypointMarkerColor(mLineEditKeypointViewerMarkerColor);
  mLineEditKeypointViewerMarkerColor->setText(color);
}

void FeatureViewerSettingsWidget::setSelectedMarkerWidth(int width)
{
  const QSignalBlocker blockerSelectKeypointViewerWidth(mSpinBoxSelectKeypointViewerMarkerWidth);
  mSpinBoxSelectKeypointViewerMarkerWidth->setValue(width);
}

void FeatureViewerSettingsWidget::setSelectedMarkerColor(const QString &color)
{
  const QSignalBlocker blockerSelectKeypointViewerColor(mLineEditSelectKeypointViewerMarkerColor);
  mLineEditSelectKeypointViewerMarkerColor->setText(color);
}

void FeatureViewerSettingsWidget::onPushButtonBackgroundColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditKeypointViewerBGColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditKeypointViewerBGColor->setText(color.name());
}

void FeatureViewerSettingsWidget::onPushButtonMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditKeypointViewerMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditKeypointViewerMarkerColor->setText(color.name());
}

void FeatureViewerSettingsWidget::onPushButtonSelectedMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditSelectKeypointViewerMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditSelectKeypointViewerMarkerColor->setText(color.name());
}

void FeatureViewerSettingsWidget::update()
{
  mSpinBoxKeypointViewerMarkerSize->setDisabled(mListWidgetKeypointsViewerMarkerType->currentRow() == 0);
}

void FeatureViewerSettingsWidget::retranslate()
{
  this->setWindowTitle(QApplication::translate("FeatureViewerSettingsWidget", "Feature viewer", nullptr));
}

void FeatureViewerSettingsWidget::clear()
{
  const QSignalBlocker blockerKeypointViewerBGColor(mLineEditKeypointViewerBGColor);
  const QSignalBlocker blockerKeypointsMarker(mListWidgetKeypointsViewerMarkerType);
  const QSignalBlocker blockerKeypointMarkerSize(mSpinBoxKeypointViewerMarkerSize);
  const QSignalBlocker blockerKeypointViewerWidth(mSpinBoxKeypointViewerMarkerWidth);
  const QSignalBlocker blockerKeypointMarkerColor(mLineEditKeypointViewerMarkerColor);
  const QSignalBlocker blockerSelectKeypointViewerWidth(mSpinBoxSelectKeypointViewerMarkerWidth);
  const QSignalBlocker blockerSelectKeypointViewerColor(mLineEditSelectKeypointViewerMarkerColor);
  
  mLineEditKeypointViewerBGColor->setText("#dcdcdc");
  mListWidgetKeypointsViewerMarkerType->setCurrentRow(0);
  mSpinBoxKeypointViewerMarkerSize->setValue(20);
  mSpinBoxKeypointViewerMarkerWidth->setValue(2);
  mLineEditKeypointViewerMarkerColor->setText("#00aa00");
  mSpinBoxSelectKeypointViewerMarkerWidth->setValue(2);
  mLineEditSelectKeypointViewerMarkerColor->setText("#e5097e");
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

  //QGroupBox *groupBoxKPVGeneral = new QGroupBox(tr("General"), this);
  //gridLayoutContentsFeaturesViewer->addWidget(groupBoxKPVGeneral);
  //QGridLayout *layoutKPVGeneral = new QGridLayout();
  //groupBoxKPVGeneral->setLayout(layoutKPVGeneral);
  gridLayoutContentsFeaturesViewer->addWidget(new QLabel(tr("Background Color:")), 0, 0, 1, 1);
  mLineEditKeypointViewerBGColor = new QLineEdit(this);
  mLineEditKeypointViewerBGColor->setText("#dcdcdc");
  gridLayoutContentsFeaturesViewer->addWidget(mLineEditKeypointViewerBGColor, 0, 1, 1, 1);
  mPushButtonKeypointViewerBGColor = new QPushButton(tr("..."), this);
  mPushButtonKeypointViewerBGColor->setMaximumSize(QSize(23, 23));
  gridLayoutContentsFeaturesViewer->addWidget(mPushButtonKeypointViewerBGColor, 0, 2, 1, 1);

  QGroupBox *groupBoxKPVMarker = new QGroupBox(tr("Marker"), this);
  gridLayoutContentsFeaturesViewer->addWidget(groupBoxKPVMarker, 1, 0, 1, 3);
  QGridLayout *layoutKPVMarker = new QGridLayout();
  groupBoxKPVMarker->setLayout(layoutKPVMarker);
  layoutKPVMarker->addWidget(new QLabel(tr("Type:")), 0, 0, 1, 1);
  mListWidgetKeypointsViewerMarkerType = new QListWidget(this);
  mListWidgetKeypointsViewerMarkerType->setIconSize(QSize(30, 30));
  mListWidgetKeypointsViewerMarkerType->setViewMode(QListWidget::ListMode);
  mListWidgetKeypointsViewerMarkerType->setResizeMode(QListWidget::Fixed);
  mListWidgetKeypointsViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint.png"), "Scale/Distance", mListWidgetKeypointsViewerMarkerType));
  mListWidgetKeypointsViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_circle.png"), "Circle", mListWidgetKeypointsViewerMarkerType));
  mListWidgetKeypointsViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_cross.png"), "Cross", mListWidgetKeypointsViewerMarkerType));
  mListWidgetKeypointsViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_diag_cross.png"), "Diagonal cross", mListWidgetKeypointsViewerMarkerType));
  mListWidgetKeypointsViewerMarkerType->setCurrentRow(0);
  layoutKPVMarker->addWidget(mListWidgetKeypointsViewerMarkerType, 0, 1, 1, 2);
  layoutKPVMarker->addWidget(new QLabel(tr("Size:")), 1, 0, 1, 1);
  mSpinBoxKeypointViewerMarkerSize = new QSpinBox(this);
  mSpinBoxKeypointViewerMarkerSize->setRange(0, 100);
  mSpinBoxKeypointViewerMarkerSize->setValue(20);
  layoutKPVMarker->addWidget(mSpinBoxKeypointViewerMarkerSize, 1, 1, 1, 2);
  layoutKPVMarker->addWidget(new QLabel(tr("Width:")), 2, 0, 1, 1);
  mSpinBoxKeypointViewerMarkerWidth = new QSpinBox(this);
  mSpinBoxKeypointViewerMarkerWidth->setRange(0, 50);
  mSpinBoxKeypointViewerMarkerWidth->setValue(2);
  layoutKPVMarker->addWidget(mSpinBoxKeypointViewerMarkerWidth, 2, 1, 1, 2);
  layoutKPVMarker->addWidget(new QLabel(tr("Color:")), 3, 0, 1, 1);
  mLineEditKeypointViewerMarkerColor = new QLineEdit(this);
  mLineEditKeypointViewerMarkerColor->setText("#e5097e");
  layoutKPVMarker->addWidget(mLineEditKeypointViewerMarkerColor, 3, 1, 1, 1);
  mPushButtonKeypointViewerMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonKeypointViewerMarkerColor->setMaximumSize(QSize(23, 23));
  layoutKPVMarker->addWidget(mPushButtonKeypointViewerMarkerColor, 3, 2, 1, 1);

  QGroupBox *groupBoxKPVSelectMarker = new QGroupBox(tr("Selected marker"), this);
  gridLayoutContentsFeaturesViewer->addWidget(groupBoxKPVSelectMarker, 2, 0, 1, 3);
  QGridLayout *layoutKPVSelectMarker = new QGridLayout();
  groupBoxKPVSelectMarker->setLayout(layoutKPVSelectMarker);
  layoutKPVSelectMarker->addWidget(new QLabel(tr("Width:")), 0, 0, 1, 1);
  mSpinBoxSelectKeypointViewerMarkerWidth = new QSpinBox(this);
  mSpinBoxSelectKeypointViewerMarkerWidth->setRange(0, 50);
  mSpinBoxSelectKeypointViewerMarkerWidth->setValue(2);
  layoutKPVSelectMarker->addWidget(mSpinBoxSelectKeypointViewerMarkerWidth, 0, 1, 1, 2);
  layoutKPVSelectMarker->addWidget(new QLabel(tr("Color:")), 1, 0, 1, 1);
  mLineEditSelectKeypointViewerMarkerColor = new QLineEdit(this);
  mLineEditSelectKeypointViewerMarkerColor->setText("#ff0000");
  layoutKPVSelectMarker->addWidget(mLineEditSelectKeypointViewerMarkerColor, 1, 1, 1, 1);
  mPushButtonSelectKeypointViewerMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonSelectKeypointViewerMarkerColor->setMaximumSize(QSize(23, 23));
  layoutKPVSelectMarker->addWidget(mPushButtonSelectKeypointViewerMarkerColor, 1, 2, 1, 1);

  gridLayoutContentsFeaturesViewer->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 3, 1, 1, 1);

  scrollKeypointsViewer->setWidget(scrollAreaWidgetContentsViewerKeypoints);
  layout->addWidget(scrollKeypointsViewer);

  retranslate();
  clear();
  update();
}

void FeatureViewerSettingsWidget::initSignalAndSlots()
{
  connect(mLineEditKeypointViewerBGColor, SIGNAL(textChanged(QString)), this,SIGNAL(backgroundColorChange(QString)));
  connect(mPushButtonKeypointViewerBGColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonBackgroundColorClicked()));
  connect(mSpinBoxKeypointViewerMarkerSize, SIGNAL(valueChanged(int)), this, SIGNAL(markerSizeChange(int)));
  connect(mSpinBoxKeypointViewerMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(markerWidthChange(int)));
  connect(mLineEditKeypointViewerMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(markerColorChange(QString)));
  connect(mPushButtonKeypointViewerMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMarkerColorClicked()));
  connect(mSpinBoxSelectKeypointViewerMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(selectedMarkerWidthChange(int)));
  connect(mLineEditSelectKeypointViewerMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(selectedMarkerColorChange(QString)));
  connect(mPushButtonSelectKeypointViewerMarkerColor, SIGNAL(clicked(bool)),                this,  SLOT(onPushButtonSelectedMarkerColorClicked()));
  connect(mListWidgetKeypointsViewerMarkerType, SIGNAL(currentRowChanged(int)), this, SIGNAL(markerTypeChange(int)));
  connect(mListWidgetKeypointsViewerMarkerType, SIGNAL(currentTextChanged(QString)), this, SLOT(update()));
}


} // namespace graphos
