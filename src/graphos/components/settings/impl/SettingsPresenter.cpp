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

#include "SettingsPresenter.h"

#include "graphos/components/settings/SettingsModel.h"
#include "graphos/components/settings/SettingsView.h"
#include "graphos/widgets/settings/FeatViewerSettingsWidget.h"
#include "graphos/widgets/settings/MatchViewerSettingsWidget.h"

#include <QLocale>

namespace graphos
{


SettingsPresenterImp::SettingsPresenterImp(SettingsView *view,
                                           SettingsModel *model)
  : SettingsPresenter(),
    mView(view),
    mModel(model)
{
    SettingsPresenterImp::init();
    SettingsPresenterImp::initSignalAndSlots();
}

void SettingsPresenterImp::open()
{

    setLanguageSettings();

    mView->setHistoryMaxSize(mModel->historyMaxSize());
    mView->setUseCuda(mModel->useCuda() && mModel->checkDevice());
    mView->setCudaEnabled(mModel->checkDevice());

    setKeypointsViewerSettings();
    setMatchesViewerSettings();

    mView->exec();
}

void SettingsPresenterImp::setFeatureViewer(FeatureViewerSettingsWidget *widget)
{
    mFeatureViewerSettingsWidget = widget;

    mView->addWidget(widget);

    connect(widget, &FeatureViewerSettingsWidget::backgroundColorChange,
            mModel, &SettingsModel::setKeypointsViewerBGColor);
    connect(widget, &FeatureViewerSettingsWidget::markerTypeChange,
            mModel, &SettingsModel::setKeypointsViewerMarkerType);
    connect(widget, &FeatureViewerSettingsWidget::markerSizeChange,
            mModel, &SettingsModel::setKeypointsViewerMarkerSize);
    connect(widget, &FeatureViewerSettingsWidget::markerWidthChange,
            mModel, &SettingsModel::setKeypointsViewerMarkerWidth);
    connect(widget, &FeatureViewerSettingsWidget::markerColorChange,
            mModel, &SettingsModel::setKeypointsViewerMarkerColor);
    connect(widget, &FeatureViewerSettingsWidget::selectedMarkerWidthChange,
            mModel, &SettingsModel::setKeypointsViewerSelectMarkerWidth);
    connect(widget, &FeatureViewerSettingsWidget::selectedMarkerColorChange,
            mModel, &SettingsModel::setKeypointsViewerSelectMarkerColor);

}

void SettingsPresenterImp::setMatchesViewer(MatchViewerSettingsWidget *widget)
{
    mMatchViewerSettingsWidget = widget;

    mView->addWidget(widget);

    connect(widget, &MatchViewerSettingsWidget::backgroundColorChange,
            mModel, &SettingsModel::setMatchesViewerBGColor);
    connect(widget, &MatchViewerSettingsWidget::markerTypeChange,
            mModel, &SettingsModel::setMatchesViewerMarkerType);
    connect(widget, &MatchViewerSettingsWidget::markerSizeChange,
            mModel, &SettingsModel::setMatchesViewerMarkerSize);
    connect(widget, &MatchViewerSettingsWidget::markerWidthChange,
            mModel, &SettingsModel::setMatchesViewerMarkerWidth);
    connect(widget, &MatchViewerSettingsWidget::markerColorChange,
            mModel, &SettingsModel::setMatchesViewerMarkerColor);
    connect(widget, &MatchViewerSettingsWidget::selectedMarkerWidthChange,
            mModel, &SettingsModel::setMatchesViewerSelectMarkerWidth);
    connect(widget, &MatchViewerSettingsWidget::selectedMarkerColorChange,
            mModel, &SettingsModel::setMatchesViewerSelectMarkerColor);
    connect(widget, &MatchViewerSettingsWidget::lineWidthChange,
            mModel, &SettingsModel::setMatchesViewerLineWidth);
    connect(widget, &MatchViewerSettingsWidget::lineColorChange,
            mModel, &SettingsModel::setMatchesViewerLineColor);
}

void SettingsPresenterImp::setLanguageSettings()
{
    QStringList languages = mModel->languages();
    QStringList langs;
    mLang.clear();

    for (auto lang_code : languages)
    {
        lang_code.truncate(lang_code.lastIndexOf('.'));
        lang_code.remove(0, lang_code.indexOf('_') + 1);
        QString lang_name = QLocale::languageToString(QLocale(lang_code).language());
        langs.push_back(lang_name);
        mLang[lang_name] = lang_code;

    }

    mView->setLanguages(langs);
}

void SettingsPresenterImp::setKeypointsViewerSettings()
{
    if (mFeatureViewerSettingsWidget) {

        mFeatureViewerSettingsWidget->setBackgroundColor(mModel->keypointsViewerBGColor());
        mFeatureViewerSettingsWidget->setMarkerType(mModel->keypointsViewerMarkerType());
        mFeatureViewerSettingsWidget->setMarkerSize(mModel->keypointsViewerMarkerSize());
        mFeatureViewerSettingsWidget->setMarkerWidth(mModel->keypointsViewerMarkerWidth());
        mFeatureViewerSettingsWidget->setMarkerColor(mModel->keypointsViewerMarkerColor());
        mFeatureViewerSettingsWidget->setSelectedMarkerColor(mModel->keypointsViewerSelectMarkerColor());
        mFeatureViewerSettingsWidget->setSelectedMarkerWidth(mModel->keypointsViewerSelectMarkerWidth());

    }
}

void SettingsPresenterImp::setMatchesViewerSettings()
{
    if (mMatchViewerSettingsWidget) {

        mMatchViewerSettingsWidget->setBackgroundColor(mModel->matchesViewerBGColor());
        mMatchViewerSettingsWidget->setMarkerType(mModel->matchesViewerMarkerType());
        mMatchViewerSettingsWidget->setMarkerSize(mModel->matchesViewerMarkerSize());
        mMatchViewerSettingsWidget->setMarkerWidth(mModel->matchesViewerMarkerWidth());
        mMatchViewerSettingsWidget->setMarkerColor(mModel->matchesViewerMarkerColor());
        mMatchViewerSettingsWidget->setSelectedMarkerWidth(mModel->matchesViewerSelectMarkerWidth());
        mMatchViewerSettingsWidget->setSelectedMarkerColor(mModel->matchesViewerSelectMarkerColor());
        mMatchViewerSettingsWidget->setLineColor(mModel->matchesViewerLineColor());
        mMatchViewerSettingsWidget->setLineWidth(mModel->matchesViewerLineWidth());

    }
}

void SettingsPresenterImp::init()
{

}

void SettingsPresenterImp::initSignalAndSlots()
{
    connect(mView, &SettingsView::languageChange,
            this, &SettingsPresenterImp::setLanguage);

    connect(mView, &SettingsView::historyMaxSizeChange,
            mModel, &SettingsModel::setHistoryMaxSize);
    connect(mView, &SettingsView::useCudaChange,
            mModel, &SettingsModel::setUseCuda);
    connect(mView, &SettingsView::imageViewerBGColorChange,
            mModel, &SettingsModel::setImageViewerBGcolor);

    connect(mView, &SettingsView::accepted,
            this, &SettingsPresenterImp::save);
    connect(mView, &SettingsView::applyChanges,
            this, &SettingsPresenterImp::save);
    connect(mView, &SettingsView::rejected,
            this, &SettingsPresenterImp::discart);
    connect(mView, &DialogView::help, 
            [&]() {
                emit help("settings.html");
            });

    connect(mModel, &SettingsModel::unsavedChanges,
            mView, &SettingsView::setUnsavedChanges);
}

void SettingsPresenterImp::setLanguage(const QString &language)
{
    QString lang_code = mLang[language];

    mModel->setLanguage(lang_code);
}

void SettingsPresenterImp::save()
{
    mModel->write();
}

void SettingsPresenterImp::discart()
{
    mModel->read();
}

} // namespace graphos
