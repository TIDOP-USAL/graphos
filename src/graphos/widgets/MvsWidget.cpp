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

#include "MvsWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>

namespace graphos
{

MvsWidget::MvsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelResolutionLevel(new QLabel(this)),
    mSpinBoxResolutionLevel(new QSpinBox(this)),
    mLabelMinResolution(new QLabel(this)),
    mSpinBoxMinResolution(new QSpinBox(this)),
    mLabelMaxResolution(new QLabel(this)),
    mSpinBoxMaxResolution(new QSpinBox(this)),
    mLabelNumberViewsFuse(new QLabel(this)),
    mSpinBoxNumberViewsFuse(new QSpinBox(this))
{
  MvsWidget::initUI();
  MvsWidget::initSignalAndSlots();
}

MvsWidget::~MvsWidget()
{

}

int MvsWidget::resolutionLevel() const
{
  return mSpinBoxResolutionLevel->value();
}

int MvsWidget::minResolution() const
{
  return mSpinBoxMinResolution->value();
}

int MvsWidget::maxResolution() const
{
  return mSpinBoxMaxResolution->value();
}

int MvsWidget::numberViewsFuse() const
{
  return mSpinBoxNumberViewsFuse->value();
}

void MvsWidget::setResolutionLevel(int resolutionLevel)
{
  const QSignalBlocker blockerResolutionLevel(mSpinBoxResolutionLevel);
  mSpinBoxResolutionLevel->setValue(resolutionLevel);
}

void MvsWidget::setMinResolution(int minResolution)
{
  const QSignalBlocker blockerMinResolution(mSpinBoxMinResolution);
  mSpinBoxMinResolution->setValue(minResolution);
}

void MvsWidget::setMaxResolution(int maxResolution)
{
  const QSignalBlocker blockerMaxResolution(mSpinBoxMaxResolution);
  mSpinBoxMaxResolution->setValue(maxResolution);
}

void MvsWidget::setNumberViewsFuse(int numberViewsFuse)
{
  const QSignalBlocker blockerNumberViewsFuse(mSpinBoxNumberViewsFuse);
  mSpinBoxNumberViewsFuse->setValue(numberViewsFuse);
}

void MvsWidget::update()
{

}

void MvsWidget::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("MvsWidget", "MVS", nullptr));
  mLabelResolutionLevel->setText(QApplication::translate("MvsWidget", "Resolution Level:"));
  mLabelMinResolution->setText(QApplication::translate("MvsWidget", "Min Resolution:"));
  mLabelMaxResolution->setText(QApplication::translate("MvsWidget", "Max Resolution:"));
  mLabelNumberViewsFuse->setText(QApplication::translate("MvsWidget", "Number Views Fuse:"));
}

void MvsWidget::clear()
{
  const QSignalBlocker blockerBoxResolutionLevel(mSpinBoxResolutionLevel);
  const QSignalBlocker blockerMinResolution(mSpinBoxMinResolution);
  const QSignalBlocker blockerMaxResolution(mSpinBoxMaxResolution);
  const QSignalBlocker blockerNumberViewsFuse(mSpinBoxNumberViewsFuse);

  mSpinBoxResolutionLevel->setValue(3);
  mSpinBoxMinResolution->setValue(500);
  mSpinBoxMaxResolution->setValue(400);
  mSpinBoxNumberViewsFuse->setValue(2);
}

void MvsWidget::initUI()
{
  this->setWindowTitle("MVS");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelResolutionLevel, 0, 0);
  mSpinBoxResolutionLevel->setRange(1, 4);
  propertiesLayout->addWidget(mSpinBoxResolutionLevel, 0, 1);

  propertiesLayout->addWidget(mLabelMinResolution, 1, 0);
  mSpinBoxMinResolution->setRange(0, 10000);
  propertiesLayout->addWidget(mSpinBoxMinResolution, 1, 1);

  propertiesLayout->addWidget(mLabelMaxResolution, 2, 0);
  mSpinBoxMaxResolution->setRange(1, 10000);
  propertiesLayout->addWidget(mSpinBoxMaxResolution, 2, 1);  

  propertiesLayout->addWidget(mLabelNumberViewsFuse, 3, 0);
  mSpinBoxNumberViewsFuse->setRange(1, 100);
  propertiesLayout->addWidget(mSpinBoxNumberViewsFuse, 3, 1); 

  MvsWidget::retranslate();
  MvsWidget::clear(); /// set default values
  MvsWidget::update();
}

void MvsWidget::initSignalAndSlots()
{
  connect(mSpinBoxResolutionLevel,          QOverload<int>::of(&QSpinBox::valueChanged),          this, &MvsWidget::resolutionLevelChanged);
  connect(mSpinBoxMinResolution,            QOverload<int>::of(&QSpinBox::valueChanged),          this, &MvsWidget::minResolutionChanged);
  connect(mSpinBoxMaxResolution,            QOverload<int>::of(&QSpinBox::valueChanged),          this, &MvsWidget::maxResolutionChanged);
  connect(mSpinBoxNumberViewsFuse,          QOverload<int>::of(&QSpinBox::valueChanged),          this, &MvsWidget::numberViewsFuseChanged);
}


} // namespace graphos