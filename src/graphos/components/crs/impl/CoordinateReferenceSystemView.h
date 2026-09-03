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

#pragma once

#include "graphos/components/crs/CoordinateReferenceSystemView.h"

#include <tidop/geotools/GeoToolsDefinitions.h>

#include <proj.h>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QTextEdit;
class QCheckBox;
class QComboBox;

namespace graphos
{

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
    QLineEdit *mLineEditVerticalCRS;
    QTreeWidget *mCRSTree;
    QTreeWidget *mVerticalCRSTree;
    QCheckBox *mCheckBoxVerticalCRS;
    QDialogButtonBox *mButtonBox;
    QTextEdit *mTextEdidCRSDetails;
    QTextEdit* mTextEdidVerticalCRSDetails;
    std::map<std::string, tl::CRSInfo> mCRSsInfo;
    std::map<std::string, tl::CRSInfo> mVerticalCRSsInfo;
};

} // namespace graphos
