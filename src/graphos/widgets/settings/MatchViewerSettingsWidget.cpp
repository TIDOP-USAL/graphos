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
  return mLineEditMatchesViewerBGColor->text();
}

int MatchViewerSettingsWidget::markerType() const
{
  return mListWidgetMatchesViewerMarkerType->currentRow();
}

int MatchViewerSettingsWidget::markerSize() const
{
  return mSpinBoxMatchesViewerMarkerSize->value();
}

int MatchViewerSettingsWidget::markerWidth() const
{
  return mSpinBoxMatchesViewerMarkerWidth->value();
}

QString MatchViewerSettingsWidget::markerColor() const
{
  return mLineEditMatchesViewerMarkerColor->text();
}

int MatchViewerSettingsWidget::selectedMarkerWidth() const
{
  return mSpinBoxSelectMatchesViewerMarkerWidth->value();
}

QString MatchViewerSettingsWidget::selectedMarkerColor() const
{
  return mLineEditSelectMatchesViewerMarkerColor->text();
}

QString MatchViewerSettingsWidget::lineColor() const
{
  return mLineEditMatchesViewerLineColor->text();
}

int MatchViewerSettingsWidget::lineWidth() const
{
  return mSpinBoxMatchesViewerLineWidth->value();
}

void MatchViewerSettingsWidget::setBackgroundColor(const QString &color)
{
  const QSignalBlocker blockerMatchesViewerBGColo(mLineEditMatchesViewerBGColor);
  mLineEditMatchesViewerBGColor->setText(color);
}

void MatchViewerSettingsWidget::setMarkerType(int type)
{
  const QSignalBlocker blockerMatchesMarker(mListWidgetMatchesViewerMarkerType);
  mListWidgetMatchesViewerMarkerType->setCurrentRow(type);
}

void MatchViewerSettingsWidget::setMarkerSize(int size)
{
  const QSignalBlocker blocker(mSpinBoxMatchesViewerMarkerSize);
  mSpinBoxMatchesViewerMarkerSize->setValue(size);
}

void MatchViewerSettingsWidget::setMarkerWidth(int width)
{
  const QSignalBlocker blockerMatchesViewerMarkerWidth(mSpinBoxMatchesViewerMarkerWidth);
  mSpinBoxMatchesViewerMarkerWidth->setValue(width);
}

void MatchViewerSettingsWidget::setMarkerColor(const QString &color)
{
  const QSignalBlocker blockerMatchesMarkerColor(mLineEditMatchesViewerMarkerColor);
  mLineEditMatchesViewerMarkerColor->setText(color);
}

void MatchViewerSettingsWidget::setSelectedMarkerWidth(int width)
{
  const QSignalBlocker blockerMatchesSelectMarkerColor(mSpinBoxSelectMatchesViewerMarkerWidth);
  mSpinBoxSelectMatchesViewerMarkerWidth->setValue(width);
}

void MatchViewerSettingsWidget::setSelectedMarkerColor(const QString &color)
{
  const QSignalBlocker blockerSelectMatchesMarkerColor(mLineEditSelectMatchesViewerMarkerColor);
  mLineEditSelectMatchesViewerMarkerColor->setText(color);
}

void MatchViewerSettingsWidget::setLineColor(const QString &color)
{
  const QSignalBlocker blockerMatchesViewerLineColor(mLineEditMatchesViewerLineColor);
  mLineEditMatchesViewerLineColor->setText(color);
}

void MatchViewerSettingsWidget::setLineWidth(int width)
{
  const QSignalBlocker blocker(mSpinBoxMatchesViewerLineWidth);
  mSpinBoxMatchesViewerLineWidth->setValue(width);
}

void MatchViewerSettingsWidget::onPushButtonMatchViewerBGColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditMatchesViewerBGColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditMatchesViewerBGColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMatchViewerMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditMatchesViewerMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditMatchesViewerMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonSelectMatchViewerMarkerColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditSelectMatchesViewerMarkerColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditSelectMatchesViewerMarkerColor->setText(color.name());
}

void MatchViewerSettingsWidget::onPushButtonMatchViewerLineColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditMatchesViewerLineColor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    mLineEditMatchesViewerLineColor->setText(color.name());
}

void MatchViewerSettingsWidget::update()
{
}

void MatchViewerSettingsWidget::retranslate()
{
  this->setWindowTitle(QApplication::translate("MatchViewerSettingsWidget", "Match viewer", nullptr));

}

void MatchViewerSettingsWidget::clear()
{
  const QSignalBlocker blockerMatchesViewerMarkerSize(mSpinBoxMatchesViewerMarkerSize);
  const QSignalBlocker blockerMatchesMarkerColor(mLineEditMatchesViewerMarkerColor);
  const QSignalBlocker blockerMatchesViewerLineColor(mLineEditMatchesViewerLineColor);
  const QSignalBlocker blocker(mSpinBoxMatchesViewerLineWidth);
  const QSignalBlocker blockerMatchesViewerBGColo(mLineEditMatchesViewerBGColor);
  const QSignalBlocker blockerMatchesMarker(mListWidgetMatchesViewerMarkerType);
  const QSignalBlocker blockerMatchesViewerMarkerWidth(mSpinBoxMatchesViewerMarkerWidth);
  const QSignalBlocker blockerSelectMatchesMarkerColor(mLineEditSelectMatchesViewerMarkerColor);

  mLineEditMatchesViewerBGColor->setText("#dcdcdc");
  mListWidgetMatchesViewerMarkerType->setCurrentRow(0);
  mSpinBoxMatchesViewerMarkerSize->setValue(20);
  mSpinBoxMatchesViewerMarkerWidth->setValue(2);
  mLineEditMatchesViewerMarkerColor->setText("#00aa00");
  mLineEditMatchesViewerLineColor->setText("#0000ff");
  mSpinBoxMatchesViewerLineWidth->setValue(2);
  mSpinBoxSelectMatchesViewerMarkerWidth->setValue(2);
  mLineEditSelectMatchesViewerMarkerColor->setText("#e5097e");
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

  //QGroupBox *groupBoxMVGeneral = new QGroupBox(tr("General"), this);
  //gridLayoutContentsMatchesViewer->addWidget(groupBoxMVGeneral);
  //QGridLayout *layoutMVGeneral = new QGridLayout();
  //groupBoxMVGeneral->setLayout(layoutMVGeneral);
  gridLayoutContentsMatchesViewer->addWidget(new QLabel(tr("Background Color:")), 0, 0, 1, 1);
  mLineEditMatchesViewerBGColor = new QLineEdit(this);
  mLineEditMatchesViewerBGColor->setText("#dcdcdc");
  gridLayoutContentsMatchesViewer->addWidget(mLineEditMatchesViewerBGColor, 0, 1, 1, 1);
  mPushButtonMatchesViewerBGColor = new QPushButton(tr("..."), this);
  mPushButtonMatchesViewerBGColor->setMaximumSize(QSize(23, 23));
  gridLayoutContentsMatchesViewer->addWidget(mPushButtonMatchesViewerBGColor, 0, 2, 1, 1);

  QGroupBox *groupBoxMVMarker = new QGroupBox(tr("Marker"), this);
  gridLayoutContentsMatchesViewer->addWidget(groupBoxMVMarker, 1, 0, 1, 3);
  QGridLayout *layoutMVMarker = new QGridLayout();
  groupBoxMVMarker->setLayout(layoutMVMarker);

  layoutMVMarker->addWidget(new QLabel(tr("Type:")), 0, 0, 1, 1);
  mListWidgetMatchesViewerMarkerType = new QListWidget(this);
  mListWidgetMatchesViewerMarkerType->setIconSize(QSize(30, 30));
  mListWidgetMatchesViewerMarkerType->setViewMode(QListWidget::ListMode);
  mListWidgetMatchesViewerMarkerType->setResizeMode(QListWidget::Fixed);
  mListWidgetMatchesViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_circle.png"), "Circle", mListWidgetMatchesViewerMarkerType));
  mListWidgetMatchesViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_cross.png"), "Cross", mListWidgetMatchesViewerMarkerType));
  mListWidgetMatchesViewerMarkerType->addItem(new QListWidgetItem(QIcon(":/ico/96/img/icons/keypoint_diag_cross.png"), "Diagonal cross", mListWidgetMatchesViewerMarkerType));
  mListWidgetMatchesViewerMarkerType->setCurrentRow(0);
  layoutMVMarker->addWidget(mListWidgetMatchesViewerMarkerType, 0, 1, 1, 2);
  layoutMVMarker->addWidget(new QLabel(tr("Size:")), 1, 0, 1, 1);
  mSpinBoxMatchesViewerMarkerSize = new QSpinBox(this);
  mSpinBoxMatchesViewerMarkerSize->setRange(0, 100);
  mSpinBoxMatchesViewerMarkerSize->setValue(20);
  layoutMVMarker->addWidget(mSpinBoxMatchesViewerMarkerSize, 1, 1, 1, 2);
  layoutMVMarker->addWidget(new QLabel(tr("Width:")), 2, 0, 1, 1);
  mSpinBoxMatchesViewerMarkerWidth = new QSpinBox(this);
  mSpinBoxMatchesViewerMarkerWidth->setRange(0, 50);
  mSpinBoxMatchesViewerMarkerWidth->setValue(2);
  layoutMVMarker->addWidget(mSpinBoxMatchesViewerMarkerWidth, 2, 1, 1, 2);
  layoutMVMarker->addWidget(new QLabel(tr("Color:")), 3, 0, 1, 1);
  mLineEditMatchesViewerMarkerColor = new QLineEdit(this);
  mLineEditMatchesViewerMarkerColor->setText("#e5097e");
  layoutMVMarker->addWidget(mLineEditMatchesViewerMarkerColor, 3, 1, 1, 1);
  mPushButtonMatchesViewerMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonMatchesViewerMarkerColor->setMaximumSize(QSize(23, 23));
  layoutMVMarker->addWidget(mPushButtonMatchesViewerMarkerColor, 3, 2, 1, 1);

  QGroupBox *groupBoxMVSelectMarker = new QGroupBox(tr("Select matches"), this);
  gridLayoutContentsMatchesViewer->addWidget(groupBoxMVSelectMarker, 2, 0, 1, 3);
  QGridLayout *layoutMVSelectMarker = new QGridLayout();
  groupBoxMVSelectMarker->setLayout(layoutMVSelectMarker);
  layoutMVSelectMarker->addWidget(new QLabel(tr("Width:")), 0, 0, 1, 1);
  mSpinBoxSelectMatchesViewerMarkerWidth = new QSpinBox(this);
  mSpinBoxSelectMatchesViewerMarkerWidth->setRange(0, 50);
  mSpinBoxSelectMatchesViewerMarkerWidth->setValue(2);
  layoutMVSelectMarker->addWidget(mSpinBoxSelectMatchesViewerMarkerWidth, 0, 1, 1, 2);
  layoutMVSelectMarker->addWidget(new QLabel(tr("Color:")), 1, 0, 1, 1);
  mLineEditSelectMatchesViewerMarkerColor = new QLineEdit(this);
  mLineEditSelectMatchesViewerMarkerColor->setText("#ff0000");
  layoutMVSelectMarker->addWidget(mLineEditSelectMatchesViewerMarkerColor, 1, 1, 1, 1);
  mPushButtonSelectMatchesViewerMarkerColor = new QPushButton(tr("..."), this);
  mPushButtonSelectMatchesViewerMarkerColor->setMaximumSize(QSize(23, 23));
  layoutMVSelectMarker->addWidget(mPushButtonSelectMatchesViewerMarkerColor, 1, 2, 1, 1);

  QGroupBox *groupBoxMVLine = new QGroupBox(tr("Line"), this);
  gridLayoutContentsMatchesViewer->addWidget(groupBoxMVLine, 3, 0, 1, 3);
  QGridLayout *layoutMVLine = new QGridLayout();
  groupBoxMVLine->setLayout(layoutMVLine);
  layoutMVLine->addWidget(new QLabel(tr("Width:")), 0, 0, 1, 1);
  mSpinBoxMatchesViewerLineWidth = new QSpinBox(this);
  mSpinBoxMatchesViewerLineWidth->setRange(0, 50);
  mSpinBoxMatchesViewerLineWidth->setValue(2);
  layoutMVLine->addWidget(mSpinBoxMatchesViewerLineWidth, 0, 1, 1, 2);
  layoutMVLine->addWidget(new QLabel(tr("Color:")), 1, 0, 1, 1);
  mLineEditMatchesViewerLineColor = new QLineEdit(this);
  mLineEditMatchesViewerLineColor->setText("#e5097e");
  layoutMVLine->addWidget(mLineEditMatchesViewerLineColor, 1, 1, 1, 1);
  mPushButtonMatchesViewerLineColor = new QPushButton(tr("..."), this);
  mPushButtonMatchesViewerLineColor->setMaximumSize(QSize(23, 23));
  layoutMVLine->addWidget(mPushButtonMatchesViewerLineColor, 1, 2, 1, 1);

  gridLayoutContentsMatchesViewer->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 4, 1, 1, 1);

  scrollMatchesViewer->setWidget(scrollAreaWidgetContentsViewerMatches);
  layout->addWidget(scrollMatchesViewer);

  retranslate();
  clear();
  update();
}

void MatchViewerSettingsWidget::initSignalAndSlots()
{
  connect(mLineEditMatchesViewerBGColor, SIGNAL(textChanged(QString)), this, SIGNAL(backgroundColorChange(QString)));
  connect(mPushButtonMatchesViewerBGColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMatchViewerBGColorClicked()));
  connect(mSpinBoxMatchesViewerMarkerSize, SIGNAL(valueChanged(int)), this, SIGNAL(markerSizeChange(int)));
  connect(mSpinBoxMatchesViewerLineWidth, SIGNAL(valueChanged(int)), this, SIGNAL(lineWidthChange(int)));
  connect(mSpinBoxMatchesViewerMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(markerWidthChange(int)));
  connect(mLineEditMatchesViewerMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(markerColorChange(QString)));
  connect(mPushButtonMatchesViewerMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMatchViewerMarkerColorClicked()));
  connect(mLineEditMatchesViewerLineColor, SIGNAL(textChanged(QString)), this, SIGNAL(lineColorChange(QString)));
  connect(mPushButtonMatchesViewerLineColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonMatchViewerLineColorClicked()));
  connect(mListWidgetMatchesViewerMarkerType, SIGNAL(currentRowChanged(int)), this, SIGNAL(markerTypeChange(int)));
  connect(mSpinBoxSelectMatchesViewerMarkerWidth, SIGNAL(valueChanged(int)), this, SIGNAL(selectedMarkerWidthChange(int)));
  connect(mLineEditSelectMatchesViewerMarkerColor, SIGNAL(textChanged(QString)), this, SIGNAL(selectedMarkerColorChange(QString)));
  connect(mPushButtonSelectMatchesViewerMarkerColor, SIGNAL(clicked(bool)), this, SLOT(onPushButtonSelectMatchViewerMarkerColorClicked()));

}


} // namespace graphos
