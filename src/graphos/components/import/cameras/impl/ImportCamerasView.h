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

#ifndef GRAPHOS_IMPORT_CAMERAS_VIEW_H
#define GRAPHOS_IMPORT_CAMERAS_VIEW_H

#include "graphos/components/import/cameras/ImportCamerasView.h"

class QDialogButtonBox;
class QTableView;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QCheckBox;

namespace graphos
{

class ImportCamerasViewImp
  : public ImportCamerasView
{

    Q_OBJECT

public:

    ImportCamerasViewImp(QWidget *parent = nullptr);
    ~ImportCamerasViewImp() override;

public slots:

    void onDelimiterChanged();

// ImportCamerasView interface

public:

    QString delimiter() const override;

public slots:

    void setItemModel(QStandardItemModel *model) override;
    void setItemModelFormatCameras(QStandardItemModel *model) override;
    void setTableHeader(const QStringList &header) override;
    void setImageColumn(const QString &imageColumn) override;
    void setXColumn(const QString &xColumn) override;
    void setYColumn(const QString &yColumn) override;
    void setZColumn(const QString &zColumn) override;
    void setQxColumn(const QString &qxColumn) override;
    void setQyColumn(const QString &qyColumn) override;
    void setQzColumn(const QString &qzColumn) override;
    void setQwColumn(const QString &qwColumn) override;
    void setOmegaColumn(const QString &omegaColumn) override;
    void setPhiColumn(const QString &phiColumn) override;
    void setKappaColumn(const QString &kappaColumn) override;
    void setYawColumn(const QString &yawColumn) override;
    void setPitchColumn(const QString &pitchColumn) override;
    void setRollColumn(const QString &rollColumn) override;
    void setParseOk(bool parseOk) override;
    void setValidInputCRS(bool valid) override;
    void setValidOutputCRS(bool valid) override;
    void setOutputCRS(const QString &crs) override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

protected:

    QGroupBox *mGroupBoxDelimiter;
    QRadioButton *mRadioButtonTab;
    QRadioButton *mRadioButtonComma;
    QRadioButton *mRadioButtonSpace;
    QRadioButton *mRadioButtonSemicolon;
    QRadioButton *mRadioButtonOther;
    QLineEdit *mLineEditOther;
    QCheckBox *mCheckBoxFieldNamesAuto;
    QLabel *mLabelSkipLines;
    QSpinBox *mSpinBoxSkipLines;
    QGroupBox *mGroupBoxColumns;
    QLabel *mLabelImageColumn;
    QComboBox *mComboBoxImageColumn;
    QGroupBox *mGroupBoxCoordinates;
    QLabel *mLabelXColumn;
    QComboBox *mComboBoxXColumn;
    QLabel *mLabelYColumn;
    QComboBox *mComboBoxYColumn;
    QLabel *mLabelZColumn;
    QComboBox *mComboBoxZColumn;
    QLabel *mLabelCrs;
    QLineEdit *mLineEditCrsInput;
    QLabel *mLabelCrsOut;
    QLineEdit *mLineEditCrsOutput;
    QGroupBox *mGroupBoxRotations;
    QGridLayout *gridLayoutRotations;
    QLabel *mLabelRotation;
    QComboBox *mComboBoxRotation;
    QStackedWidget *mStackedWidget;
    QWidget *pageQuaternions;
    QWidget *pageYawPitchRoll;
    QWidget *pageOmegaPhiKappa;
    QLabel *mLabelQxColumn;
    QComboBox *mComboBoxQxColumn;
    QLabel *mLabelQyColumn;
    QComboBox *mComboBoxQyColumn;
    QLabel *mLabelQzColumn;
    QComboBox *mComboBoxQzColumn;
    QLabel *mLabelQwColumn;
    QComboBox *mComboBoxQwColumn;
    QLabel *mLabelYawColumn;
    QComboBox *mComboBoxYawColumn;
    QLabel *mLabelPitchColumn;
    QComboBox *mComboBoxPitchColumn;
    QLabel *mLabelRollColumn;
    QComboBox *mComboBoxRollColumn;
    QLabel *mLabelOmegaColumn;
    QComboBox *mComboBoxOmegaColumn;
    QLabel *mLabelPhiColumn;
    QComboBox *mComboBoxPhiColumn;
    QLabel *mLabelKappaColumn;
    QComboBox *mComboBoxKappaColumn;
    QLabel *mLabelPreview;
    QTableView *mTableViewImportCameras;
    QLabel *mLabelCameras;
    QTableView *mTableViewFormatCameras;
    QDialogButtonBox *mButtonBox;
    //QString mProjectPath;
    bool bParseOk;
    bool bValidCrsIn;
    bool bValidCrsOut;
};

} // namespace graphos

#endif // GRAPHOS_IMPORT_CAMERAS_VIEW_H
