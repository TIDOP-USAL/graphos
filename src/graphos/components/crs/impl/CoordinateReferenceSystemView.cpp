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


namespace graphos
{

CoordinateReferenceSystemViewImp::CoordinateReferenceSystemViewImp(QWidget *parent)
  : CoordinateReferenceSystemView(parent)
{
    this->initUI();
    this->initSignalAndSlots();
}

CoordinateReferenceSystemViewImp::~CoordinateReferenceSystemViewImp()
{

}

void CoordinateReferenceSystemViewImp::filterSRS()
{
    QString searchText = searchBar->text();
    for (int i = 0; i < srsTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *category = srsTree->topLevelItem(i);
        for (int j = 0; j < category->childCount(); ++j) {
            QTreeWidgetItem *item = category->child(j);
            bool match = item->text(0).contains(searchText, Qt::CaseInsensitive);
            item->setHidden(!match);
        }
    }
}

void CoordinateReferenceSystemViewImp::showSRSDetails()
{
    QList<QTreeWidgetItem *> selectedItems = srsTree->selectedItems();
    if (selectedItems.isEmpty()) return;

    QTreeWidgetItem *selectedItem = selectedItems.first();
    QString selectedText = selectedItem->text(0);
    int index = srsEntries.indexOf(selectedText);
    if (index < 0 || index >= crsInfoList.size()) return;

    const CRSInfo &crsInfo = crsInfoList[index];
    QString details;
    details += QString("Name: %1\n").arg(crsInfo.name);
    details += QString("Code: %1\n").arg(crsInfo.code);
    details += QString("Type: %1\n").arg(crsInfo.type);
    details += QString("Auth name: %1\n").arg(crsInfo.auth_name);
    details += QString("Area name: %1\n").arg(crsInfo.area_name);
    details += QString("Projection method name: %1\n").arg(crsInfo.projection_method_name);
    details += QString("Deprecated: %1\n").arg(crsInfo.deprecated ? "Yes" : "No");

    srsDetails->setText(details);

    emit crs_changed(QString("%1:%2").arg(crsInfo.auth_name, crsInfo.code));
}

void CoordinateReferenceSystemViewImp::loadSRS()
{
    srsTree->clear();
    srsEntries.clear();
    crsInfoList.clear();

    PJ_CONTEXT *ctx = proj_context_create();
    int crs_count;
    PROJ_CRS_INFO **crs_info_list = proj_get_crs_info_list_from_database(ctx, "EPSG", nullptr, &crs_count);

    for (int i = 0; i < crs_count; i++) {
        CRSInfo crsInfo;
        crsInfo.auth_name = crs_info_list[i]->auth_name;
        crsInfo.name = crs_info_list[i]->name;
        crsInfo.code = crs_info_list[i]->code;
        crsInfo.type = pjTypeToString(crs_info_list[i]->type);
        crsInfo.deprecated = crs_info_list[i]->deprecated;
        crsInfo.area_name = crs_info_list[i]->area_name;
        crsInfo.projection_method_name = crs_info_list[i]->projection_method_name;

        if (!showDeprecatedCheckBox->isChecked() && crsInfo.deprecated) {
            continue;
        }

        QString entry = QString("%1 (%2)").arg(crsInfo.name).arg(crsInfo.code);
        srsEntries << entry;
        crsInfoList.append(crsInfo);
    }

    proj_crs_info_list_destroy(crs_info_list);
    proj_context_destroy(ctx);

    //srsList->addItems(srsEntries);

    QTreeWidgetItem *geodeticItem = new QTreeWidgetItem(srsTree, QStringList() << "Geodetic CRS");
    QTreeWidgetItem *projectedItem = new QTreeWidgetItem(srsTree, QStringList() << "Projected CRS");

    for (const CRSInfo &crsInfo : crsInfoList) {
        QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << QString("%1 (%2)").arg(crsInfo.name).arg(crsInfo.code));
        if (crsInfo.type.contains("Geographic") || crsInfo.type.contains("Geocentric")) {
            geodeticItem->addChild(item);
        } else if (crsInfo.type.contains("Projected")) {
            projectedItem->addChild(item);
        }
    }

    srsTree->expandAll();
}

QString CoordinateReferenceSystemViewImp::pjTypeToString(PJ_TYPE type)
{
    switch (type)
    {
    case PJ_TYPE_UNKNOWN:
        return "Unknown";
    case PJ_TYPE_ELLIPSOID:
        return "Ellipsoid";
    case PJ_TYPE_PRIME_MERIDIAN:
        return "Prime Meridian";
    case PJ_TYPE_GEODETIC_REFERENCE_FRAME:
        return "Geodetic Reference Frame";
    case PJ_TYPE_DYNAMIC_GEODETIC_REFERENCE_FRAME:
        return "Dynamic Geodetic Reference Frame";
    case PJ_TYPE_VERTICAL_REFERENCE_FRAME:
        return "Vertical Reference Frame";
    case PJ_TYPE_DYNAMIC_VERTICAL_REFERENCE_FRAME:
        return "Dynamic Vertical Reference Frame";
    case PJ_TYPE_DATUM_ENSEMBLE:
        return "Datum Ensemble";
    case PJ_TYPE_CRS:
        return "Coordinate Reference System";
    case PJ_TYPE_GEODETIC_CRS:
        return "Geodetic Coordinate Reference System";
    case PJ_TYPE_GEOCENTRIC_CRS:
        return "Geocentric Coordinate Reference System";
    case PJ_TYPE_GEOGRAPHIC_CRS:
        return "Geographic Coordinate Reference System";
    case PJ_TYPE_GEOGRAPHIC_2D_CRS:
        return "Geographic 2D Coordinate Reference System";
    case PJ_TYPE_GEOGRAPHIC_3D_CRS:
        return "Geographic 3D Coordinate Reference System";
    case PJ_TYPE_VERTICAL_CRS:
        return "Vertical Coordinate Reference System";
    case PJ_TYPE_PROJECTED_CRS:
        return "Projected Coordinate Reference System";
    case PJ_TYPE_COMPOUND_CRS:
        return "Compound Coordinate Reference System";
    case PJ_TYPE_TEMPORAL_CRS:
        return "Temporal Coordinate Reference System";
    case PJ_TYPE_ENGINEERING_CRS:
        return "Engineering Coordinate Reference System";
    //case PJ_TYPE_PARAMETRIC_CRS:
    //    return "Parametric Coordinate Reference System";
    case PJ_TYPE_OTHER_CRS:
        return "Other Coordinate Reference System";
    case PJ_TYPE_CONVERSION:
        return "Conversion";
    case PJ_TYPE_TRANSFORMATION:
        return "Transformation";
    case PJ_TYPE_CONCATENATED_OPERATION:
        return "Concatenated Operation";
    case PJ_TYPE_OTHER_COORDINATE_OPERATION:
        return "Other Coordinate Operation";
    //case PJ_TYPE_DATUM:
    //    return "Datum";
    //case PJ_TYPE_COORDINATE_OPERATION:
    //    return "Coordinate Operation";
    //case PJ_TYPE_COORDINATE_SYSTEM:
    //    return "Coordinate System";
    //case PJ_TYPE_AXIS:
    //    return "Axis";
    //case PJ_TYPE_UNIT_OF_MEASURE:
    //    return "Unit of Measure";
    //case PJ_TYPE_ELLIPSOIDAL_CS:
    //    return "Ellipsoidal Coordinate System";
    //case PJ_TYPE_CARTESIAN_CS:
    //    return "Cartesian Coordinate System";
    //case PJ_TYPE_SPHERICAL_CS:
    //    return "Spherical Coordinate System";
    //case PJ_TYPE_VERTICAL_CS:
    //    return "Vertical Coordinate System";
    //case PJ_TYPE_TEMPORAL_CS:
    //    return "Temporal Coordinate System";
    //case PJ_TYPE_PARAMETRIC_CS:
    //    return "Parametric Coordinate System";
    //case PJ_TYPE_ORDINATE_CS:
    //    return "Ordinate Coordinate System";
    default:
        return "Unknown";
    }
}

void CoordinateReferenceSystemViewImp::initUI()
{
    this->setObjectName(QString("CoordinateReferenceSystemView"));
    this->resize(380, 250);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabelSearch = new QLabel(this);
    gridLayout->addWidget(mLabelSearch, 0, 0, 1, 1);
    searchBar = new QLineEdit(this);
    gridLayout->addWidget(searchBar, 0, 1, 1, 1);
    showDeprecatedCheckBox = new QCheckBox("Show Deprecated", this);
    gridLayout->addWidget(showDeprecatedCheckBox, 1, 0, 1, 2);
    srsTree = new QTreeWidget(this);
    srsTree->setColumnCount(1);
    srsTree->setHeaderLabel("Coordinate Reference Systems");
    gridLayout->addWidget(srsTree, 2, 0, 1, 2);

    srsDetails = new QTextEdit(this);
    srsDetails->setReadOnly(true);
    gridLayout->addWidget(srsDetails, 3, 0, 1, 2);


    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

    loadSRS();

    this->retranslate();
    this->clear();
    this->update();
}

void CoordinateReferenceSystemViewImp::initSignalAndSlots()
{
    connect(searchBar, &QLineEdit::textChanged, this, &CoordinateReferenceSystemViewImp::filterSRS);
    connect(showDeprecatedCheckBox, &QCheckBox::stateChanged, this, &CoordinateReferenceSystemViewImp::loadSRS);
    connect(srsTree, &QTreeWidget::itemSelectionChanged, this, &CoordinateReferenceSystemViewImp::showSRSDetails);

    connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &QDialog::accept);
    connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void CoordinateReferenceSystemViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("CoordinateReferenceSystemView", "Coordinate Reference System"));
    mLabelSearch->setText(QApplication::translate("SearchView", "Search:"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CoordinateReferenceSystemView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("CoordinateReferenceSystemView", "Apply"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CoordinateReferenceSystemView", "Help"));
}

void CoordinateReferenceSystemViewImp::clear()
{

}

void CoordinateReferenceSystemViewImp::update()
{
}



} // namespace graphos


