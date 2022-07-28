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

void SettingsViewImp::onPushButtonImageViewerBGColorClicked()
{
  QColor color = QColorDialog::getColor(QColor(mLineEditImageViewerBGcolor->text()), this, "Pick a color",  QColorDialog::DontUseNativeDialog);
  if (color.isValid())
    emit imageViewerBGColorChange(color.name());
}

void SettingsViewImp::initUI()
{
  setObjectName(QString("SettingsView"));
  resize(750, 450);


  QGridLayout *layout = new QGridLayout();
  setLayout(layout);

  mListWidget->setMaximumSize(QSize(250, 16777215));
  mListWidget->addItem(tr("General"));
  mListWidget->addItem(tr("Image Viewer"));
  mListWidget->addItem(tr("Tools"));
  layout->addWidget(mListWidget, 0, 0, 1, 1);

  layout->addWidget(mStackedWidget, 0, 1, 1, 3);

  /* General */
  QWidget *pageGeneral = new QWidget(this);
  QGridLayout *gridLayoutGeneral = new QGridLayout(pageGeneral);

  gridLayoutGeneral->addWidget(mLabelLanguages, 0, 0, 1, 1);
  gridLayoutGeneral->addWidget(mLanguages, 0, 1, 1, 1);

  gridLayoutGeneral->addWidget(mLabelHistoryMaxSize, 1, 0, 1, 1);
  mHistoryMaxSize->setRange(1, 50);
  mHistoryMaxSize->setValue(10);
  gridLayoutGeneral->addWidget(mHistoryMaxSize, 1, 1, 1, 1);

  gridLayoutGeneral->addWidget(mLabelUseCuda, 2, 0, 1, 1);
  gridLayoutGeneral->addWidget(mCheckBoxUseCuda, 2, 1, 1, 1);

  QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  gridLayoutGeneral->addItem(verticalSpacer, 3, 0, 1, 1);

  mStackedWidget->addWidget(pageGeneral);

  /* Viewer */
  QWidget *pageImageViewer = new QWidget(this);
  QGridLayout *gridLayoutViewer = new QGridLayout(pageImageViewer);
  gridLayoutViewer->setContentsMargins(0, 0, 0, 0);

  QTabWidget *tabWidgetImageViewer = new QTabWidget(pageImageViewer);

  QWidget *tabImageViewerGeneral = new QWidget(this);
  QGridLayout *gridLayoutTabImageViewerGeneral = new QGridLayout(tabImageViewerGeneral);
  gridLayoutTabImageViewerGeneral->setContentsMargins(0, 0, 0, 0);
  QScrollArea *scrollAreaImageViewerGeneral = new QScrollArea(tabImageViewerGeneral);
  scrollAreaImageViewerGeneral->setWidgetResizable(true);
  scrollAreaImageViewerGeneral->setFrameShape(QFrame::Shape::NoFrame);
  QWidget *scrollAreaWidgetContentsImageViewerGeneral = new QWidget(this);
  QGridLayout *gridLayoutContentsImageViewerGeneral = new QGridLayout(scrollAreaWidgetContentsImageViewerGeneral);
  gridLayoutContentsImageViewerGeneral->addWidget(mLabelImageViewerBGcolor, 0, 0, 1, 1);
  mLineEditImageViewerBGcolor = new QLineEdit(tabImageViewerGeneral);
  mLineEditImageViewerBGcolor->setText("#dcdcdc");
  gridLayoutContentsImageViewerGeneral->addWidget(mLineEditImageViewerBGcolor, 0, 1, 1, 1);
  mPushButtonImageViewerBGcolor = new QPushButton(tr("..."), tabImageViewerGeneral);
  mPushButtonImageViewerBGcolor->setMaximumSize(QSize(23, 23));
  gridLayoutContentsImageViewerGeneral->addWidget(mPushButtonImageViewerBGcolor, 0, 2, 1, 1);

  scrollAreaImageViewerGeneral->setWidget(scrollAreaWidgetContentsImageViewerGeneral);
  gridLayoutTabImageViewerGeneral->addWidget(scrollAreaImageViewerGeneral);

  tabWidgetImageViewer->addTab(tabImageViewerGeneral, QString(tr("General")));

  gridLayoutViewer->addWidget(tabWidgetImageViewer, 0, 0, 1, 1);

  mStackedWidget->addWidget(pageImageViewer);

  /* Tools */
  QWidget *pageTools = new QWidget(this);
  QGridLayout *gridLayoutTools = new QGridLayout(pageTools);
  gridLayoutTools->setContentsMargins(0, 0, 0, 0);

  mTabWidgetTools = new QTabWidget(pageTools);

  QWidget *tabToolsGeneral = new QWidget(this);
  QGridLayout *gridLayoutTabToolsGeneral = new QGridLayout(tabToolsGeneral);
  gridLayoutTabToolsGeneral->setContentsMargins(0, 0, 0, 0);
  QScrollArea *scrollAreaToolsGeneral = new QScrollArea(tabToolsGeneral);
  scrollAreaToolsGeneral->setWidgetResizable(true);
  scrollAreaToolsGeneral->setFrameShape(QFrame::Shape::NoFrame);
  QWidget *scrollAreaWidgetContentsToolsGeneral = new QWidget(this);
  QGridLayout *gridLayoutContentsToolsGeneral = new QGridLayout(scrollAreaWidgetContentsToolsGeneral);
  scrollAreaToolsGeneral->setWidget(scrollAreaWidgetContentsToolsGeneral);
  gridLayoutTabToolsGeneral->addWidget(scrollAreaToolsGeneral, 0, 0, 1, 1);
  mTabWidgetTools->addTab(tabToolsGeneral, QString(tr("General")));


  gridLayoutTools->addWidget(mTabWidgetTools, 0, 0, 1, 1);
  mStackedWidget->addWidget(pageTools);

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
          this, &SettingsViewImp::onPushButtonImageViewerBGColorClicked);
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
  const QSignalBlocker blockerImageViewerBGcolor(mLineEditImageViewerBGcolor);

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

QString SettingsViewImp::activeLanguage() const
{
  return mLanguages->currentText();
}

int SettingsViewImp::historyMaxSize() const
{
  return mHistoryMaxSize->value();
}

QString SettingsViewImp::imageViewerBGColor() const
{
  return mLineEditImageViewerBGcolor->text();
}

bool SettingsViewImp::useCuda() const
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
