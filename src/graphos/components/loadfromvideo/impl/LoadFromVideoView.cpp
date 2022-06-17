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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "LoadFromVideoView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>


namespace graphos
{

LoadFromVideoViewImp::LoadFromVideoViewImp(QWidget *parent)
  : LoadFromVideoView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

LoadFromVideoViewImp::~LoadFromVideoViewImp()
{

}

void LoadFromVideoViewImp::initUI()
{
  this->setObjectName(QString("LoadFromVideoView"));
  this->resize(380, 250);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  mLabelVideo = new QLabel(this);
  gridLayout->addWidget(mLabelVideo, 0, 0, 1, 1);
  mComboBoxVideo = new QComboBox(this);
  gridLayout->addWidget(mComboBoxVideo, 0, 1, 1, 1);
  mLabelSkipFrames = new QLabel(this);
  gridLayout->addWidget(mLabelSkipFrames, 1, 0, 1, 1);
  mSpinBoxSkipFrames = new QSpinBox(this);
  gridLayout->addWidget(mSpinBoxSkipFrames, 1, 1, 1, 1);


  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 7, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void LoadFromVideoViewImp::initSignalAndSlots()
{
  connect(mSpinBoxSkipFrames, QOverload<int>::of(&QSpinBox::valueChanged), this, &LoadFromVideoView::SkipFramesChanged);

  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &LoadFromVideoView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void LoadFromVideoViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("LoadFromVideoView", "Load from Video"));
  
  mLabelVideo->setText(QApplication::translate("VideoView", "Video:"));
  mLabelSkipFrames->setText(QApplication::translate("SkipFramesView", "Skip Frames:"));

  
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("LoadFromVideoView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("LoadFromVideoView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("LoadFromVideoView", "Help"));
}

void LoadFromVideoViewImp::clear()
{

}

void LoadFromVideoViewImp::update()
{
}

QString LoadFromVideoViewImp::Video() const 
{
  return mComboBoxVideo->currentText();
}

int LoadFromVideoViewImp::SkipFrames() const 
{
  return mSpinBoxSkipFrames->value();
}


void LoadFromVideoViewImp::setVideo(const QString &Video) 
{
  const QSignalBlocker blocker(mComboBoxVideo);
  mComboBoxVideo->setCurrentText(Video);
}

void LoadFromVideoViewImp::setSkipFrames(int SkipFrames) 
{
  const QSignalBlocker blocker(mSpinBoxSkipFrames);
  mSpinBoxSkipFrames->setValue(SkipFrames);
}



} // namespace graphos


