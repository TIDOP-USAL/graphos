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

#ifndef GRAPHOS_SETTINGS_VIEW_H
#define GRAPHOS_SETTINGS_VIEW_H

#include "graphos/components/settings/SettingsView.h"

class QListWidget;
class QStackedWidget;
class QComboBox;
class QDialogButtonBox;
class QTabWidget;
class QGridLayout;
class QSpinBox;
class QLineEdit;
class QCheckBox;
class QLabel;

namespace graphos
{


class SettingsViewImp
  : public SettingsView
{
    Q_OBJECT

public:

    SettingsViewImp(QWidget *parent = nullptr);
    ~SettingsViewImp() override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

// SettingsView interface

public:

    auto activeLanguage() const -> QString override;
    auto historyMaxSize() const -> int override;
    auto imageViewerBGColor() const -> QString override;
    auto useCuda() const -> bool override;

public slots:

    void setPage(int page) override;
    void setLanguages(const QStringList &languages) override;
    void setActiveLanguage(const QString &language) override;
    void setHistoryMaxSize(int size) override;
    void setImageViewerBGcolor(const QString &color) override;
    void setUseCuda(bool active) override;
    void setCudaEnabled(bool enabled) override;
    void setUnsavedChanges(bool unsaveChanges) override;
    void addWidget(QWidget *widget) override;

protected:

    QListWidget *mListWidget;
    QStackedWidget *mStackedWidget;
    QLabel *mLabelLanguages;
    QComboBox *mLanguages;
    QLabel *mLabelHistoryMaxSize;
    QSpinBox *mHistoryMaxSize;
    QTabWidget *mTabWidgetTools;
    QLabel *mLabelUseCuda;
    QCheckBox *mCheckBoxUseCuda;
    QLabel *mLabelImageViewerBGcolor;
    QLineEdit *mLineEditImageViewerBGcolor;
    QPushButton *mPushButtonImageViewerBGcolor;
    QDialogButtonBox *mButtonBox;

    bool bUnsaveChanges;
};


} // namespace graphos

#endif // GRAPHOS_SETTINGS_VIEW_H
