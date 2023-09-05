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

#include "MeshView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QLabel>
#include <QSpinBox>


namespace graphos
{

MeshViewImp::MeshViewImp(QWidget *parent)
  : MeshView(parent)
{
    this->initUI();
    this->initSignalAndSlots();
}

MeshViewImp::~MeshViewImp()
{

}

void MeshViewImp::initUI()
{
    this->setObjectName(QString("MeshView"));
    this->resize(380, 250);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabelDepth = new QLabel(this);
    gridLayout->addWidget(mLabelDepth, 0, 0, 1, 1);
    mSpinBoxDepth = new QSpinBox(this);
    gridLayout->addWidget(mSpinBoxDepth, 0, 1, 1, 1);

    mLabelSolveDepth = new QLabel(this);
    gridLayout->addWidget(mLabelSolveDepth, 1, 0, 1, 1);

    mSpinBoxSolveDepth = new QSpinBox(this);
    gridLayout->addWidget(mSpinBoxSolveDepth, 1, 1, 1, 1);

    mLabelBoundaryType = new QLabel(this);
    gridLayout->addWidget(mLabelBoundaryType, 2, 0, 1, 1);
    mComboBoxBoundaryType = new QComboBox(this);
    mComboBoxBoundaryType->addItem("free");
    mComboBoxBoundaryType->addItem("Dirichlet");
    mComboBoxBoundaryType->addItem("Neumann");
    gridLayout->addWidget(mComboBoxBoundaryType, 2, 1, 1, 1);

    //mLabelWidth = new QLabel(this);
    //gridLayout->addWidget(mLabelWidth, 3, 0, 1, 1);
    //mSpinBoxWidth = new QSpinBox(this);
    //gridLayout->addWidget(mSpinBoxWidth, 3, 1, 1, 1);

    //mLabelFullDepth = new QLabel(this);
    //gridLayout->addWidget(mLabelFullDepth, 4, 0, 1, 1);
    //mSpinBoxFullDepth = new QSpinBox(this);
    //gridLayout->addWidget(mSpinBoxFullDepth, 4, 1, 1, 1);


    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void MeshViewImp::initSignalAndSlots()
{
    connect(mSpinBoxDepth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MeshViewImp::depthChanged);
    connect(mSpinBoxSolveDepth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MeshViewImp::solveDepthChanged);
    //connect(mSpinBoxWidth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MeshViewImp::widthChanged);
    //connect(mSpinBoxFullDepth, QOverload<int>::of(&QSpinBox::valueChanged), this, &MeshViewImp::fullDepthChanged);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &MeshViewImp::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void MeshViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("MeshView", "Mesh"));

    mLabelDepth->setText(QApplication::translate("DepthView", "Maximum reconstruction depth:"));
    mLabelSolveDepth->setText(QApplication::translate("SolveDepthView", "Maximum solution depth:"));
    mLabelBoundaryType->setText(QApplication::translate("BoundaryTypeView", "Boundary Type:"));
    //mLabelWidth->setText(QApplication::translate("WidthView", "Grid width:"));
    //mLabelFullDepth->setText(QApplication::translate("FullDepthView", "Full Depth:"));


    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("MeshView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("MeshView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("MeshView", "Help"));
}

void MeshViewImp::clear()
{
    mSpinBoxDepth->setMinimum(11);
    mSpinBoxSolveDepth->setMinimum(10);
}

void MeshViewImp::update()
{
}

int MeshViewImp::depth() const
{
    return mSpinBoxDepth->value();
}

int MeshViewImp::solveDepth() const
{
    return mSpinBoxSolveDepth->value();
}

QString MeshViewImp::boundaryType() const
{
    return mComboBoxBoundaryType->currentText();
}

//int MeshViewImp::width() const 
//{
//  return mSpinBoxWidth->value();
//}

//int MeshViewImp::fullDepth() const 
//{
//  return mSpinBoxFullDepth->value();
//}

void MeshViewImp::setDepth(int Depth)
{
    const QSignalBlocker blocker(mSpinBoxDepth);
    mSpinBoxDepth->setValue(Depth);
}

void MeshViewImp::setSolveDepth(int SolveDepth)
{
    const QSignalBlocker blocker(mSpinBoxSolveDepth);
    mSpinBoxSolveDepth->setValue(SolveDepth);
}

void MeshViewImp::setBoundaryType(const QString &BoundaryType)
{
    const QSignalBlocker blocker(mComboBoxBoundaryType);
    mComboBoxBoundaryType->setCurrentText(BoundaryType);
}

//void MeshViewImp::setWidth(int width) 
//{
//  const QSignalBlocker blocker(mSpinBoxWidth);
//  mSpinBoxWidth->setValue(width);
//}

//void MeshViewImp::setFullDepth(int FullDepth) 
//{
//  const QSignalBlocker blocker(mSpinBoxFullDepth);
//  mSpinBoxFullDepth->setValue(FullDepth);
//}



} // namespace graphos


