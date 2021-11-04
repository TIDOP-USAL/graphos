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

#include "FeatureMatchingView.h"

#include "graphos/core/utils.h"

#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QApplication>

namespace graphos
{

FeatureMatchingViewImp::FeatureMatchingViewImp(QWidget *parent)
  : FeatureMatchingView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

FeatureMatchingViewImp::~FeatureMatchingViewImp()
{

}

void FeatureMatchingViewImp::initUI()
{
  this->setObjectName(QString("FeatureMatchingView"));
  //this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(400, 300);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mLabelMatcher = new QLabel(this);
  gridLayout->addWidget(mLabelMatcher, 0, 0, 1, 1);
  mComboBoxMatcher = new QComboBox(this);
  gridLayout->addWidget(mComboBoxMatcher, 0, 1, 1, 1);

  QWidget *widgetFeatureMatching = new QWidget();
  mGridLayoutMatcher = new QGridLayout(widgetFeatureMatching);
  mGridLayoutMatcher->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(widgetFeatureMatching, 1, 0, 1, 2);

  gridLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 2, 0, 1, 2);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 3, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void FeatureMatchingViewImp::initSignalAndSlots()
{
  connect(mComboBoxMatcher,  &QComboBox::currentTextChanged, this, &FeatureMatchingView::matchMethodChange);

  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &FeatureMatchingViewImp::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);

}

void FeatureMatchingViewImp::clear()
{
}

void FeatureMatchingViewImp::update()
{
  int count = mComboBoxMatcher->count();
  mLabelMatcher->setVisible(count > 1);
  mComboBoxMatcher->setVisible(count > 1);
}

void FeatureMatchingViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("FeatureMatchingView", "Feature Matching"));
  mLabelMatcher->setText(QApplication::translate("FeatureMatchingView", "Feature Matching Method:"));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("FeatureMatchingView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("FeatureMatchingView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("FeatureMatchingView", "Help"));
}

void FeatureMatchingViewImp::addMatchMethod(QWidget *matchMethod)
{
  mComboBoxMatcher->addItem(matchMethod->windowTitle());
  mGridLayoutMatcher->addWidget(matchMethod, 0, 0, 1, 2);
  matchMethod->setVisible(false);

  this->update();
}

QString FeatureMatchingViewImp::currentMatchMethod() const
{
  return mComboBoxMatcher->currentText();
}

void FeatureMatchingViewImp::setCurrentMatchMethod(const QString &matchMethodName)
{
  const QSignalBlocker blocker(mComboBoxMatcher);
  mComboBoxMatcher->setCurrentText(matchMethodName);

  for (int idx = 0; idx < mGridLayoutMatcher->count(); idx++){
    QLayoutItem * const item = mGridLayoutMatcher->itemAt(idx);
    if(dynamic_cast<QWidgetItem *>(item)){
      if (item->widget()->windowTitle().compare(matchMethodName) == 0)
        item->widget()->setVisible(true);
      else
        item->widget()->setVisible(false);
    }
  }
}

} // End namespace graphos
