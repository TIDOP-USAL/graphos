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

#include "CoordinateReferenceSystemView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>
#include <QTextEdit>
#include <QTreeWidget>
#include <QCheckBox>

#include <gdal.h>
#include <ogr_spatialref.h>

#include <tidop/GeoTools/GeoTools.h>
#include <tidop/GeoTools/CRSsTools.h>
#include <tidop/GeoTools/impl/CRSsToolsDefinitions.h>


namespace graphos
{

CoordinateReferenceSystemViewImp::CoordinateReferenceSystemViewImp(QWidget *parent)
  : CoordinateReferenceSystemView(parent)
{
    CoordinateReferenceSystemViewImp::initUI();
    CoordinateReferenceSystemViewImp::initSignalAndSlots();
}

CoordinateReferenceSystemViewImp::~CoordinateReferenceSystemViewImp()
{

}

void CoordinateReferenceSystemViewImp::apply()
{
    QString crsEpsgCode = mLineEditCRS->text();
    QString verticalCrsEpsgCode = mLineEditVerticalCRS->text();
    QString crsId = CRS_ID_STRING_EPSG_PREFIX;
    crsId += crsEpsgCode;
    if (!verticalCrsEpsgCode.isEmpty())
    {
        crsId += "+";
        crsId += verticalCrsEpsgCode;
    }
    emit crs_changed(crsId);
    accept();
}

void CoordinateReferenceSystemViewImp::filterCRS()
{
    QString searchText = mLineEditCRS->text();
    for (int i = 0; i < mCRSTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *category = mCRSTree->topLevelItem(i);
        for (int j = 0; j < category->childCount(); ++j) {
            QTreeWidgetItem *item = category->child(j);
            bool match = item->text(0).contains(searchText, Qt::CaseInsensitive);
            item->setHidden(!match);
        }
    }
}

void CoordinateReferenceSystemViewImp::filterVerticalCRS()
{
    QString searchText = mLineEditVerticalCRS->text();
    for (int i = 0; i < mVerticalCRSTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* category = mVerticalCRSTree->topLevelItem(i);
        for (int j = 0; j < category->childCount(); ++j) {
            QTreeWidgetItem* item = category->child(j);
            bool match = item->text(0).contains(searchText, Qt::CaseInsensitive);
            item->setHidden(!match);
        }
    }
}

void CoordinateReferenceSystemViewImp::showCRSDetails()
{
    QList<QTreeWidgetItem *> selectedItems = mCRSTree->selectedItems();
    QString crsId;
    if (selectedItems.size() > 0)
    {
        QTreeWidgetItem* selectedItem = selectedItems.first();
        QString selectedText = selectedItem->text(0);
        crsId = selectedItem->data(0, Qt::UserRole).toString();
    }
    if (crsId.isEmpty())
    {
        update();
        return;
    }

    const tl::CRSInfo &crsInfo = mCRSsInfo[crsId.toStdString()];
    QString details;
    details += QString("Name: %1\n").arg(QString::fromStdString(crsInfo.name));
    details += QString("Code: %1\n").arg(QString::fromStdString(crsInfo.code));
    details += QString("Type: %1\n").arg(QString::fromStdString(crsInfo.type));
    details += QString("Auth name: %1\n").arg(QString::fromStdString(crsInfo.auth_name));
    details += QString("Area name: %1\n").arg(QString::fromStdString(crsInfo.area_name));
    details += QString("Projection method name: %1\n").arg(QString::fromStdString(crsInfo.projection_method_name));
    details += QString("Deprecated: %1\n").arg(QString::fromStdString(crsInfo.deprecated ? "Yes" : "No"));

    mTextEdidCRSDetails->setText(details);
    mLineEditCRS->setText(QString::fromStdString(crsInfo.code));
    update();

}

void CoordinateReferenceSystemViewImp::showVerticalCRSDetails()
{
    QList<QTreeWidgetItem*> selectedItems = mVerticalCRSTree->selectedItems();
    bool crsSelected = false;
    if (selectedItems.size() >0)
    {
        QTreeWidgetItem* selectedItem = selectedItems.first();
        QString selectedText = selectedItem->text(0);
        QString crsId = selectedItem->data(0, Qt::UserRole).toString();
        if (!crsId.isEmpty())
            crsSelected = true;
    }
    if (!crsSelected)
    {
        mLineEditVerticalCRS->clear();
        mTextEdidVerticalCRSDetails->clear();
        return;
    }

    QTreeWidgetItem* selectedItem = selectedItems.first();
    QString selectedText = selectedItem->text(0);
    QString crsId = selectedItem->data(0, Qt::UserRole).toString();
    const tl::CRSInfo& crsInfo = mVerticalCRSsInfo[crsId.toStdString()];
    QString details;
    details += QString("Name: %1\n").arg(QString::fromStdString(crsInfo.name));
    details += QString("Code: %1\n").arg(QString::fromStdString(crsInfo.code));
    details += QString("Type: %1\n").arg(QString::fromStdString(crsInfo.type));
    details += QString("Auth name: %1\n").arg(QString::fromStdString(crsInfo.auth_name));
    details += QString("Area name: %1\n").arg(QString::fromStdString(crsInfo.area_name));
    details += QString("Projection method name: %1\n").arg(QString::fromStdString(crsInfo.projection_method_name));
    details += QString("Deprecated: %1\n").arg(QString::fromStdString(crsInfo.deprecated ? "Yes" : "No"));

    mTextEdidVerticalCRSDetails->setText(details);
    mLineEditVerticalCRS->setText(QString::fromStdString(crsInfo.code));
}

void CoordinateReferenceSystemViewImp::loadCRS()
{
    mCRSTree->clear();
    mCRSsInfo.clear();
    mVerticalCRSTree->clear();
    mVerticalCRSsInfo.clear();

    tl::GeoTools* ptrGeoTools = tl::GeoTools::getInstance();
    ptrGeoTools->ptrCRSsTools()->getCRSsFor2dApplications(mCRSsInfo);

    QTreeWidgetItem *geodeticItem = new QTreeWidgetItem(mCRSTree, QStringList() << "Geodetic CRS");
    QTreeWidgetItem *projectedItem = new QTreeWidgetItem(mCRSTree, QStringList() << "Projected CRS");
    for (auto const& x : mCRSsInfo)
    {
        std::string crsId = x.first;
        tl::CRSInfo crsInfo = x.second;
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << QString("%1 (%2)")
            .arg(QString::fromStdString(crsInfo.name)).arg(QString::fromStdString(crsInfo.code)));
        item->setData(0, Qt::UserRole, QString::fromStdString(crsId));
        if (crsInfo.type == CRS_TYPE_PROJ_PROJECTED)
            projectedItem->addChild(item);
        else
            geodeticItem->addChild(item);
    }
    mCRSTree->expandAll();
}

void CoordinateReferenceSystemViewImp::loadVerticalCRS()
{
    mVerticalCRSTree->clear();
    //mVerticalCRSsInfo.clear();
    mTextEdidVerticalCRSDetails->clear();
    mLineEditVerticalCRS->setEnabled(mCheckBoxVerticalCRS->isChecked());
    mVerticalCRSTree->setEnabled(mCheckBoxVerticalCRS->isChecked());
    mTextEdidVerticalCRSDetails->setEnabled(mCheckBoxVerticalCRS->isChecked());
    if (!mCheckBoxVerticalCRS->isChecked())
    {
        return;
    }

    QList<QTreeWidgetItem*> selectedItems = mCRSTree->selectedItems();
    if (selectedItems.isEmpty()) return;

    QTreeWidgetItem* selectedItem = selectedItems.first();
    QString selectedText = selectedItem->text(0);
    QString crsId = selectedItem->data(0, Qt::UserRole).toString();

    tl::GeoTools* ptrGeoTools = tl::GeoTools::getInstance();

    ptrGeoTools->ptrCRSsTools()->getCRSsVertical(crsId.toStdString(), mVerticalCRSsInfo);

    QTreeWidgetItem* verticalItem = new QTreeWidgetItem(mVerticalCRSTree, QStringList() << "Vertical CRS");
    for (auto const& x : mVerticalCRSsInfo)
    {
        std::string crsId = x.first;
        tl::CRSInfo crsInfo = x.second;
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QString("%1 (%2)")
            .arg(QString::fromStdString(crsInfo.name)).arg(QString::fromStdString(crsInfo.code)));
        item->setData(0, Qt::UserRole, QString::fromStdString(crsId));
        verticalItem->addChild(item);
    }
    mVerticalCRSTree->expandAll();
}

void CoordinateReferenceSystemViewImp::initUI()
{
    this->setObjectName(QString("CoordinateReferenceSystemView"));
    this->resize(800, 400);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabel2d3dCRS = new QLabel(this);
    gridLayout->addWidget(mLabel2d3dCRS, 0, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    gridLayout->addWidget(mLineEditCRS, 0, 1, 1, 1);
    mCheckBoxVerticalCRS = new QCheckBox(this);
    gridLayout->addWidget(mCheckBoxVerticalCRS, 0, 2, 1, 1);
    mLineEditVerticalCRS = new QLineEdit(this);
    gridLayout->addWidget(mLineEditVerticalCRS, 0, 3, 1, 1);
    mCRSTree = new QTreeWidget(this);
    mCRSTree->setColumnCount(1);
    mCRSTree->setHeaderLabel("2D/3D CRS");
    gridLayout->addWidget(mCRSTree, 2, 0, 1, 2);
    mVerticalCRSTree = new QTreeWidget(this);
    mVerticalCRSTree->setColumnCount(1);
    gridLayout->addWidget(mVerticalCRSTree, 2, 2, 1, 2);

    mTextEdidCRSDetails = new QTextEdit(this);
    mTextEdidCRSDetails->setReadOnly(true);
    gridLayout->addWidget(mTextEdidCRSDetails, 3, 0, 1, 2);
    mTextEdidVerticalCRSDetails = new QTextEdit(this);
    mTextEdidVerticalCRSDetails->setReadOnly(true);
    gridLayout->addWidget(mTextEdidVerticalCRSDetails, 3, 2, 1, 2);


    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 4, 0, 1, 4);

    loadCRS();

    this->retranslate();
    this->clear();
    this->update();
}

void CoordinateReferenceSystemViewImp::initSignalAndSlots()
{
    connect(mLineEditVerticalCRS, &QLineEdit::textChanged, this, &CoordinateReferenceSystemViewImp::filterVerticalCRS);
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &CoordinateReferenceSystemViewImp::filterCRS);
    //connect(mLineEditCRS, &QLineEdit::textChanged, this, &CoordinateReferenceSystemViewImp::update);
    connect(mCheckBoxVerticalCRS, &QCheckBox::stateChanged, this, &CoordinateReferenceSystemViewImp::loadVerticalCRS);
    connect(mCRSTree, &QTreeWidget::itemSelectionChanged, this, &CoordinateReferenceSystemViewImp::showCRSDetails);
    connect(mVerticalCRSTree, &QTreeWidget::itemSelectionChanged, this, &CoordinateReferenceSystemViewImp::showVerticalCRSDetails);

    connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &CoordinateReferenceSystemViewImp::apply);
    connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void CoordinateReferenceSystemViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("CoordinateReferenceSystemComponent", "Coordinate Reference System"));
    mLabel2d3dCRS->setText(QApplication::translate("CoordinateReferenceSystemComponent", "2D/3D CRS:"));
    mCheckBoxVerticalCRS->setText(QApplication::translate("CoordinateReferenceSystemComponent", "Set vertical CRS:"));
    mVerticalCRSTree->setHeaderLabel(QApplication::translate("CoordinateReferenceSystemComponent", "Vertical CRS"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CoordinateReferenceSystemComponent", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("CoordinateReferenceSystemComponent", "Apply"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CoordinateReferenceSystemComponent", "Help"));
}

void CoordinateReferenceSystemViewImp::clear()
{
}

void CoordinateReferenceSystemViewImp::update()
{
    QList<QTreeWidgetItem*> selectedItems = mCRSTree->selectedItems();
    bool crsSelected = false;
    if (selectedItems.size() > 0)
    {
        QTreeWidgetItem* selectedItem = selectedItems.first();
        QString selectedText = selectedItem->text(0);
        QString crsId = selectedItem->data(0, Qt::UserRole).toString();
        if (!crsId.isEmpty())
            crsSelected = true;
    }
    if (!crsSelected)
    {
        mCheckBoxVerticalCRS->setChecked(false);
        mCheckBoxVerticalCRS->setEnabled(false);
        mTextEdidCRSDetails->clear();
        mLineEditCRS->clear();
    }
    else
    {
        mCheckBoxVerticalCRS->setEnabled(true);
    }
    mVerticalCRSTree->clear();
    mVerticalCRSsInfo.clear();
    mTextEdidVerticalCRSDetails->clear();
    mLineEditVerticalCRS->setEnabled(mCheckBoxVerticalCRS->isChecked());
    mVerticalCRSTree->setEnabled(mCheckBoxVerticalCRS->isChecked());
    mTextEdidVerticalCRSDetails->setEnabled(mCheckBoxVerticalCRS->isChecked());
    mButtonBox->button(QDialogButtonBox::Apply)->setEnabled(!mLineEditCRS->text().isEmpty());
}



} // namespace graphos


