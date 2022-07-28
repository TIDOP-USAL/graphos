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

MatchViewerSettingsWidget::MatchViewerSettingsWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
  initUI();
  initSignalAndSlots();
}

QString MatchViewerSettingsWidget::backgroundColor() const
{
  return mLineEditBackgroundColor->text();
}

int MatchViewerSettingsWidget::markerType() const
{
  return mListWidgetMarkerType->currentRow();
}

int MatchViewerSettingsWidget::markerSize() const
{
  return mSpinBoxMarkerSize->value();
}

int MatchViewerSettingsWidget::markerWidth() const
{
  return mSpinBoxMarkerWidth->value();
}

QString MatchViewerSettingsWidget::markerColor() const
{
  return mLineEditMarkerColor->text();
}

int MatchViewerSettingsWidget::selectedMarkerWidth() const
{
  return mSpinBoxSelectMarkerWidth->value();
}

QString MatchViewerSettingsWidget::selectedMarkerColor() const
{
  return mLineEditSelectMarkerColor->text();
}

QString MatchViewerSettingsWidget::lineColor() const
{
  return mLineEditLineColor->text();
}

int MatchViewerSettingsWidget::lineWidth() const
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
  QColor color = QColorDialog::getColor(QColor(mLineEditBackgroundColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditBackgroundColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonSelectMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditSelectMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditSelectMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMatchViewerLineColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditLineColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
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

void MatchViewerSettingsWidget::initUI()
{

  QGridLayout *layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  
  QScrollArea *scrollMatchesViewer = new QScrollArea(this);
  scrollMatchesViewer->setWidgetResizable(true);
  scrollMatchesViewer->setFrameShape(QFrame::Shape::NoFrame);
  QWidget *scrollAreaWidgetContentsViewerMatches = new QWidget(this);

  QGridLayout *gridLayoutContentsMatchesViewer = new QGridLayout(scrollAreaWidgetContentsViewerMatches);

  mLabelBackgroundColor = new QLabel(this);
  gridLayoutContentsMatchesViewer->addWidget(mLabelBackgroundColor, 0, 0, 1, 1);
  mLineEditBackgroundColor = new QLineEdit(this);
  mLineEditBackgroundColor->setText("#dcdcdc");
  gridLayoutContentsMatchesViewer->addWidget(mLineEditBackgroundColor, 0, 1, 1, 1);
  mPushButtonBackgroundColor = new QPushButton(tr("..."), this);
  mPushButtonBackgroundColor->setMaximumSize(QSize(23, 23));
  gridLayoutContentsMatchesViewer->addWidget(mPushButtonBackgroundColor, 0, 2, 1, 1);

  /* Marker */

  mGroupBoxMVMarker = new QGroupBox(this);
  gridLayoutContentsMatchesViewer->addWidget(mGroupBoxMVMarker, 1, 0, 1, 3);
  QGridLayout *layoutMVMarker = new QGridLayout();
  mGroupBoxMVMarker->setLayout(layoutMVMarker);

  mLabelMarkerType = new QLabel(this);
  layoutMVMarker->addWidget(mLabelMarkerType, 0, 0, 1, 1);
  mListWidgetMarkerType = new QListWidget(this);
  mListWidgetMarkerType->setIconSize(QSize(30, 30));
  mListWidgetMarkerType->setViewMode(QListWidget::ListMode);
  mListWidgetMarkerType->setResizeMode(QListWidget::Fixed);
  mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_circle.png"), "Circle", mListWidgetMarkerType));
  mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_cross.png"), "Cross", mListWidgetMarkerType));
  mListWidgetMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_diag_cross.png"), "Diagonal cross", mListWidgetMarkerType));
  mListWidgetMarkerType->setCurrentRow(0);
  layoutMVMarker->addWidget(mListWidgetMarkerType, 0, 1, 1, 2);

  mLabelMarkerSize = new QLabel(this);
  layoutMVMarker->addWidget(mLabelMarkerSize, 1, 0, 1, 2);
  mSpinBoxMarkerSize = new QSpinBox(this);
  mSpinBoxMarkerSize->setRange(0, 100);
  mSpinBoxMarkerSize->setValue(20);
  layoutMVMarker->addWidget(mSpinBoxMarkerSize, 1, 1, 1, 2);

  mLabelMarkerWidth = new QLabel(this);
  layoutMVMarker->addWidget(mLabelMarkerWidth, 2, 0, 1, 1);
  mSpinBoxMarkerWidth = new QSpinBox(this);
  mSpinBoxMarkerWidth->setRange(0, 50);
  mSpinBoxMarkerWidth->setValue(2);
  layoutMVMarker->addWidget(mSpinBoxMarkerWidth, 2, 1, 1, 2);

  mLabelMarkerColor = new QLabel(this);
  layoutMVMarker->addWidget(mLabelMarkerColor, 3, 0, 1, 1);
  mLineEditMarkerColor = new QLineEdit(this);
  mLineEditMarkerColor->setText("#e5097e");
  layoutMVMarker->addWidget(mLineEditMarkerColor, 3, 1, 1, 1);
  mPushButtonMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonMarkerColor->setMaximumSize(QSize(23, 23));
  layoutMVMarker->addWidget(mPushButtonMarkerColor, 3, 2, 1, 1);

  /* Select matches */

  mGroupBoxSelectMatches = new QGroupBox(this);
  gridLayoutContentsMatchesViewer->addWidget(mGroupBoxSelectMatches, 2, 0, 1, 3);
  QGridLayout *layoutMVSelectMarker = new QGridLayout();
  mGroupBoxSelectMatches->setLayout(layoutMVSelectMarker);

  mLabelSelectMarkerWidth = new QLabel(this);
  layoutMVSelectMarker->addWidget(mLabelSelectMarkerWidth, 0, 0, 1, 1);
  mSpinBoxSelectMarkerWidth = new QSpinBox(this);
  mSpinBoxSelectMarkerWidth->setRange(0, 50);
  mSpinBoxSelectMarkerWidth->setValue(2);
  layoutMVSelectMarker->addWidget(mSpinBoxSelectMarkerWidth, 0, 1, 1, 2);

  mLabelSelectMarkerColor = new QLabel(this);
  layoutMVSelectMarker->addWidget(mLabelSelectMarkerColor, 1, 0, 1, 1);
  mLineEditSelectMarkerColor = new QLineEdit(this);
  mLineEditSelectMarkerColor->setText("#ff0000");
  layoutMVSelectMarker->addWidget(mLineEditSelectMarkerColor, 1, 1, 1, 1);
  mPushButtonSelectMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonSelectMarkerColor->setMaximumSize(QSize(23, 23));
  layoutMVSelectMarker->addWidget(mPushButtonSelectMarkerColor, 1, 2, 1, 1);

  /* Line */

  mGroupBoxLine = new QGroupBox(this);
  gridLayoutContentsMatchesViewer->addWidget(mGroupBoxLine, 3, 0, 1, 3);
  QGridLayout *layoutMVLine = new QGridLayout();
  mGroupBoxLine->setLayout(layoutMVLine);

  mLabelLineWidth = new QLabel(this);
  layoutMVLine->addWidget(mLabelLineWidth, 0, 0, 1, 1);
  mSpinBoxLineWidth = new QSpinBox(this);
  mSpinBoxLineWidth->setRange(0, 50);
  mSpinBoxLineWidth->setValue(2);
  layoutMVLine->addWidget(mSpinBoxLineWidth, 0, 1, 1, 2);

  mLabelLineColor = new QLabel(this);
  layoutMVLine->addWidget(mLabelLineColor, 1, 0, 1, 1);
  mLineEditLineColor = new QLineEdit(this);
  mLineEditLineColor->setText("#e5097e");
  layoutMVLine->addWidget(mLineEditLineColor, 1, 1, 1, 1);
  mPushButtonLineColor = new QPushButton(tr("..."), this);
  mPushButtonLineColor->setMaximumSize(QSize(23, 23));
  layoutMVLine->addWidget(mPushButtonLineColor, 1, 2, 1, 1);

  gridLayoutContentsMatchesViewer->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding), 4, 1, 1, 1);

  scrollMatchesViewer->setWidget(scrollAreaWidgetContentsViewerMatches);
  layout->addWidget(scrollMatchesViewer);

  retranslate();
  clear();
  update();
}

void MatchViewerSettingsWidget::initSignalAndSlots()
{
  connect(mLineEditBackgroundColor, SIGNAL(textChanged(QString)), this,SIGNAL(backgroundColorChange(QString)));
  connect(mPushButtonBackgroundColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonBackgroundColorClicked()));
  connect(mSpinBoxMarkerSize, SIGNAL(valueChanged(int)), this, SIGNAL(markerSizeChange(int)));
  connect(mSpinBoxMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(markerWidthChange(int)));
  connect(mLineEditMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(markerColorChange(QString)));
  connect(mPushButtonMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMarkerColorClicked()));
  connect(mSpinBoxSelectMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(selectedMarkerWidthChange(int)));
  connect(mLineEditSelectMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(selectedMarkerColorChange(QString)));
  connect(mPushButtonSelectMarkerColor, SIGNAL(clicked(bool)),                this,  SLOT(onPushButtonSelectMarkerColorClicked()));
  connect(mListWidgetMarkerType, SIGNAL(currentRowChanged(int)), this, SIGNAL(markerTypeChange(int)));
  connect(mListWidgetMarkerType, SIGNAL(currentTextChanged(QString)), this, SLOT(update()));
}


} // namespace graphos
