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


#ifndef GRAPHOS_CRS_VIEW_H
#define GRAPHOS_CRS_VIEW_H

#include "graphos/components/crs/CoordinateReferenceSystemView.h"

#include <tidop/GeoTools/GeoToolsDefinitions.h>

#include <proj.h>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QStringList;
class QTextEdit;
class QCheckBox;
class QComboBox;

namespace graphos
{

//struct CRSInfo 
//{
//    QString auth_name;
//    QString name;
//    QString code;
//    QString type;
//    QString area_name;
//    QString projection_method_name;
//    bool deprecated;
//};

class CoordinateReferenceSystemViewImp
  : public CoordinateReferenceSystemView
{

    Q_OBJECT

public:

    CoordinateReferenceSystemViewImp(QWidget *parent = nullptr);
    ~CoordinateReferenceSystemViewImp() override;

private slots:

    void filterCRS();
    void filterVerticalCRS();
    void showCRSDetails();
    void showVerticalCRSDetails();

private:

    void loadCRS();
    void loadVerticalCRS();
    //QString pjTypeToString(PJ_TYPE type);

// DialogView

private:

    void initUI();
    void initSignalAndSlots();

public slots:

    void clear();

private slots:
    void apply();
    void update();
    void retranslate();

protected:

    QLabel *mLabel2d3dCRS;
    QLineEdit *mLineEditCRS;
    QLineEdit* mLineEditVerticalCRS;
    QTreeWidget *mCRSTree;
    QTreeWidget* mVerticalCRSTree;
    QCheckBox *mCheckBoxVerticalCRS;
    QDialogButtonBox *mButtonBox;
    QTextEdit *mTextEdidCRSDetails;
    QTextEdit* mTextEdidVerticalCRSDetails;
    //QList<CRSInfo> crsInfoList;
    std::map<std::string, tl::CRSInfo> mCRSsInfo;
    std::map<std::string, tl::CRSInfo> mVerticalCRSsInfo;
};

} // namespace graphos

#endif // GRAPHOS_CRS_VIEW_H
