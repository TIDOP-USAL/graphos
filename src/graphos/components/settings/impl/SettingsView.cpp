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

#include "SettingsView.h"

#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QColorDialog>
#include <QApplication>

namespace graphos
{


SettingsViewImp::SettingsViewImp(QWidget *parent)
  : SettingsView(parent),
    mListWidget(new QListWidget(this)),
    mStackedWidget(new QStackedWidget(this)),
    mLabelLanguages(new QLabel(this)),
    mLanguages(new QComboBox(this)),
    mLabelHistoryMaxSize(new QLabel(this)),
    mHistoryMaxSize(new QSpinBox(this)),
    mTabWidgetTools(nullptr),
    mLabelUseCuda(new QLabel(this)),
    mCheckBoxUseCuda(new QCheckBox(this)),
    mLabelImageViewerBGcolor(new QLabel(this)),
    mButtonBox(new QDialogButtonBox(this)),
    bUnsaveChanges(false)
{
    SettingsViewImp::initUI();
    SettingsViewImp::initSignalAndSlots();
}

SettingsViewImp::~SettingsViewImp()
{

}

void SettingsViewImp::initUI()
{
    setObjectName(QString("SettingsView"));
    resize(750, 450);


    auto layout = new QGridLayout();
    setLayout(layout);

    mListWidget->setMaximumSize(QSize(250, 16777215));
    mListWidget->addItem(tr("General"));
    mListWidget->addItem(tr("Image Viewer"));
    mListWidget->addItem(tr("Tools"));
    layout->addWidget(mListWidget, 0, 0, 1, 1);

    layout->addWidget(mStackedWidget, 0, 1, 1, 3);

    /* General */
    auto page_general = new QWidget(this);
    auto grid_layout_general = new QGridLayout(page_general);

    grid_layout_general->addWidget(mLabelLanguages, 0, 0, 1, 1);
    grid_layout_general->addWidget(mLanguages, 0, 1, 1, 1);

    grid_layout_general->addWidget(mLabelHistoryMaxSize, 1, 0, 1, 1);
    mHistoryMaxSize->setRange(1, 50);
    mHistoryMaxSize->setValue(10);
    grid_layout_general->addWidget(mHistoryMaxSize, 1, 1, 1, 1);

    grid_layout_general->addWidget(mLabelUseCuda, 2, 0, 1, 1);
    grid_layout_general->addWidget(mCheckBoxUseCuda, 2, 1, 1, 1);

    auto vertical_spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    grid_layout_general->addItem(vertical_spacer, 3, 0, 1, 1);

    mStackedWidget->addWidget(page_general);

    /* Viewer */
    auto page_image_viewer = new QWidget(this);
    auto grid_layout_viewer = new QGridLayout(page_image_viewer);
    grid_layout_viewer->setContentsMargins(0, 0, 0, 0);

    auto tab_widget_image_viewer = new QTabWidget(page_image_viewer);

    auto tab_image_viewer_general = new QWidget(this);
    auto grid_layout_tab_image_viewer_general = new QGridLayout(tab_image_viewer_general);
    grid_layout_tab_image_viewer_general->setContentsMargins(0, 0, 0, 0);
    auto scroll_area_image_viewer_general = new QScrollArea(tab_image_viewer_general);
    scroll_area_image_viewer_general->setWidgetResizable(true);
    scroll_area_image_viewer_general->setFrameShape(QFrame::Shape::NoFrame);
    auto scroll_area_widget_contents_image_viewer_general = new QWidget(this);
    auto grid_layout_contents_image_viewer_general = new QGridLayout(scroll_area_widget_contents_image_viewer_general);
    grid_layout_contents_image_viewer_general->addWidget(mLabelImageViewerBGcolor, 0, 0, 1, 1);
    mLineEditImageViewerBGcolor = new QLineEdit(tab_image_viewer_general);
    mLineEditImageViewerBGcolor->setText("#dcdcdc");
    grid_layout_contents_image_viewer_general->addWidget(mLineEditImageViewerBGcolor, 0, 1, 1, 1);
    mPushButtonImageViewerBGcolor = new QPushButton(tr("..."), tab_image_viewer_general);
    mPushButtonImageViewerBGcolor->setMaximumSize(QSize(23, 23));
    grid_layout_contents_image_viewer_general->addWidget(mPushButtonImageViewerBGcolor, 0, 2, 1, 1);

    scroll_area_image_viewer_general->setWidget(scroll_area_widget_contents_image_viewer_general);
    grid_layout_tab_image_viewer_general->addWidget(scroll_area_image_viewer_general);

    tab_widget_image_viewer->addTab(tab_image_viewer_general, QString(tr("General")));

    grid_layout_viewer->addWidget(tab_widget_image_viewer, 0, 0, 1, 1);

    mStackedWidget->addWidget(page_image_viewer);

    /* Tools */
    auto page_tools = new QWidget(this);
    auto grid_layout_tools = new QGridLayout(page_tools);
    grid_layout_tools->setContentsMargins(0, 0, 0, 0);

    mTabWidgetTools = new QTabWidget(page_tools);

    auto tab_tools_general = new QWidget(this);
    auto grid_layout_tab_tools_general = new QGridLayout(tab_tools_general);
    grid_layout_tab_tools_general->setContentsMargins(0, 0, 0, 0);
    auto scroll_area_tools_general = new QScrollArea(tab_tools_general);
    scroll_area_tools_general->setWidgetResizable(true);
    scroll_area_tools_general->setFrameShape(QFrame::Shape::NoFrame);
    auto scroll_area_widget_contents_tools_general = new QWidget(this);
    scroll_area_tools_general->setWidget(scroll_area_widget_contents_tools_general);
    grid_layout_tab_tools_general->addWidget(scroll_area_tools_general, 0, 0, 1, 1);
    mTabWidgetTools->addTab(tab_tools_general, QString(tr("General")));


    grid_layout_tools->addWidget(mTabWidgetTools, 0, 0, 1, 1);
    mStackedWidget->addWidget(page_tools);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
                                   QDialogButtonBox::Apply | QDialogButtonBox::Help);

    layout->addWidget(mButtonBox, 1, 0, 1, 4);

    this->retranslate();
    this->clear();
    this->update();
}

void SettingsViewImp::initSignalAndSlots()
{
    connect(mListWidget, &QListWidget::currentRowChanged,
        mStackedWidget, &QStackedWidget::setCurrentIndex);
    connect(mLanguages, &QComboBox::currentTextChanged,
        this, &SettingsViewImp::languageChange);
    connect(mHistoryMaxSize, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &SettingsViewImp::historyMaxSizeChange);
    connect(mLineEditImageViewerBGcolor, &QLineEdit::textChanged,
        this, &SettingsViewImp::imageViewerBGColorChange);
    connect(mPushButtonImageViewerBGcolor, &QAbstractButton::clicked,
           [&]() {
               QColor color = QColorDialog::getColor(QColor(mLineEditImageViewerBGcolor->text()), 
                                                     this,
                                                     "Pick a color",
                                                     QColorDialog::DontUseNativeDialog);
               if (color.isValid())
                   emit imageViewerBGColorChange(color.name());
           });
    connect(mCheckBoxUseCuda, &QAbstractButton::clicked,
        this, &SettingsViewImp::useCudaChange);

    connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked,
        this, &SettingsView::applyChanges);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked,
        this, &DialogView::help);
}

void SettingsViewImp::clear()
{
    const QSignalBlocker blocker1(mLanguages);
    const QSignalBlocker blocker_image_viewer_b_gcolor(mLineEditImageViewerBGcolor);

    mLanguages->clear();
    mHistoryMaxSize->setValue(10);
    mLineEditImageViewerBGcolor->setText("#dcdcdc");
    mCheckBoxUseCuda->setChecked(false);
}

void SettingsViewImp::update()
{
    mButtonBox->button(QDialogButtonBox::Apply)->setEnabled(bUnsaveChanges);
}

void SettingsViewImp::retranslate()
{
    setWindowTitle(QApplication::translate("SettingsView", "Settings", nullptr));
    mLabelHistoryMaxSize->setText(QApplication::translate("SettingsView", "History Max Size", nullptr));
    mLabelLanguages->setText(QApplication::translate("SettingsView", "Language", nullptr));
    mLabelUseCuda->setText(QApplication::translate("SettingsView", "Use Cuda", nullptr));
    mLabelImageViewerBGcolor->setText(QApplication::translate("SettingsView", "Background Color:", nullptr));

    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("SettingsView", "Ok", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("SettingsView", "Cancel", nullptr));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("SettingsView", "Apply", nullptr));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("SettingsView", "Help", nullptr));
}

auto SettingsViewImp::activeLanguage() const -> QString
{
    return mLanguages->currentText();
}

auto SettingsViewImp::historyMaxSize() const -> int
{
    return mHistoryMaxSize->value();
}

auto SettingsViewImp::imageViewerBGColor() const -> QString
{
    return mLineEditImageViewerBGcolor->text();
}

auto SettingsViewImp::useCuda() const -> bool
{
    return mCheckBoxUseCuda->isEnabled() && mCheckBoxUseCuda->isChecked();
}

void SettingsViewImp::setPage(int page)
{
    mListWidget->setCurrentRow(page);
}

void SettingsViewImp::setLanguages(const QStringList &languages)
{
    const QSignalBlocker blocker(mLanguages);

    mLanguages->clear();
    mLanguages->insertItems(0, languages);
}

void SettingsViewImp::setActiveLanguage(const QString &language)
{
    const QSignalBlocker blocker(mLanguages);
    mLanguages->setCurrentText(language);
}

void SettingsViewImp::setHistoryMaxSize(int size)
{
    const QSignalBlocker blocker(mHistoryMaxSize);
    mHistoryMaxSize->setValue(size);
}

void SettingsViewImp::setImageViewerBGcolor(const QString &color)
{
    const QSignalBlocker blockerImageViewerBGcolor(mLineEditImageViewerBGcolor);
    mLineEditImageViewerBGcolor->setText(color);
}

void SettingsViewImp::setUseCuda(bool active)
{
    mCheckBoxUseCuda->setChecked(active);
}

void SettingsViewImp::setCudaEnabled(bool enabled)
{
    mCheckBoxUseCuda->setEnabled(enabled);
}

void SettingsViewImp::setUnsavedChanges(bool unsaveChanges)
{
    bUnsaveChanges = unsaveChanges;
    update();
}

void SettingsViewImp::addWidget(QWidget *widget)
{
    mListWidget->addItem(widget->windowTitle());
    mStackedWidget->addWidget(widget);
}

} // namespace graphos
