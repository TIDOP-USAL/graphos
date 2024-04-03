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

#ifndef GRAPHOS_ABOUT_VIEW_H
#define GRAPHOS_ABOUT_VIEW_H

#include "graphos/components/about/AboutView.h"

class QGridLayout;
class QLabel;
class QTabWidget;
class QTabWidget;
class QTextEdit;
class QListWidget;
class QStackedWidget;
class QPlainTextEdit;
class QDialogButtonBox;

namespace graphos
{

class AboutViewImp
  : public AboutView
{
    Q_OBJECT

public:

    explicit AboutViewImp(QWidget *parent = nullptr);
    ~AboutViewImp() override;

// AboutView

public:

    void setVersion(const QString &version) override;
    void setLicence(const QString &licence) override;
    void addLicence(const QString &product, const QString &licence) override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

private:

    QLabel *mLabelGraphosLogo;
    QTabWidget *mTabWidget;
    QWidget *mTabGraphosLicence;
    QGridLayout *gridLayout_4;
    QTextEdit *mTextEditGraphosLicence;
    QWidget *mTabThirdPartyLiceses;
    QGridLayout *gridLayout_3;
    QListWidget *mListWidgetThirdPartyLiceses;
    QStackedWidget *mStackedWidgetThirdPartyLicenses;
    QLabel *mLabelGraphosVersion;
    QDialogButtonBox *mButtonBox;
};

} // namespace graphos

#endif // GRAPHOS_ABOUT_VIEW_H
