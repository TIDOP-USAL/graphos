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

#include "StartPageWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QSplitter>
#include <QCommandLinkButton>
#include <QListWidget>

namespace graphos
{

StartPageWidget::StartPageWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
  StartPageWidget::initUI();
  StartPageWidget::initSignalAndSlots();
}

void StartPageWidget::setHistory(const QStringList &history)
{
  const QSignalBlocker blocker(mListWidgetRecentProjects);
  mListWidgetRecentProjects->clear();
  mListWidgetRecentProjects->addItems(history);
}

void StartPageWidget::update()
{
}

void StartPageWidget::retranslate()
{
  this->setWindowTitle(QApplication::translate("StartPage", "Start Page", nullptr));
  mCommandLinkButtonNewProject->setText(QApplication::translate("StartPage", "New Project", nullptr));
  mCommandLinkButtonOpenProject->setText(QApplication::translate("StartPage", "Open Project", nullptr));
  mCommandLinkButtonSettings->setText(QApplication::translate("StartPage", "Settings", nullptr));
  mLabelRecentProjects->setText(QApplication::translate("StartPage", "Recent Projects", nullptr));
  mCommandLinkButtonClearHistory->setText(QApplication::translate("StartPage", "Clear History", nullptr));
}

void StartPageWidget::clear()
{
  const QSignalBlocker blocker(mListWidgetRecentProjects);
  mListWidgetRecentProjects->clear();
}

void StartPageWidget::initUI()
{

  QGridLayout *layout = new QGridLayout(this);
  layout->setSpacing(6);
  layout->setContentsMargins(11, 11, 11, 11);

  
  mLabelGraphosLogo = new QLabel(this);
  mLabelGraphosLogo->setMaximumSize(QSize(16777215, 79));
  mLabelGraphosLogo->setPixmap(QPixmap(QString::fromUtf8(":/images/img/graphos.png")));

  layout->addWidget(mLabelGraphosLogo, 0, 0, 1, 1);

  mSplitter = new QSplitter(this);
  mSplitter->setOrientation(Qt::Horizontal);
  QWidget *layoutWidgetButtons = new QWidget(mSplitter);
  QVBoxLayout *verticalLayout = new QVBoxLayout(layoutWidgetButtons);
  verticalLayout->setSpacing(6);
  verticalLayout->setContentsMargins(11, 11, 11, 11);
  verticalLayout->setContentsMargins(9, 9, 9, 9);
  mCommandLinkButtonNewProject = new QCommandLinkButton(layoutWidgetButtons);
  mCommandLinkButtonNewProject->setIcon(QIcon::fromTheme("new-project"));
  mCommandLinkButtonNewProject->setIconSize(QSize(48, 48));

  verticalLayout->addWidget(mCommandLinkButtonNewProject);

  mCommandLinkButtonOpenProject = new QCommandLinkButton(layoutWidgetButtons);
  mCommandLinkButtonOpenProject->setIcon(QIcon::fromTheme("open-project"));
  mCommandLinkButtonOpenProject->setIconSize(QSize(48, 48));

  verticalLayout->addWidget(mCommandLinkButtonOpenProject);

  QFrame *line = new QFrame(layoutWidgetButtons);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  verticalLayout->addWidget(line);

  mCommandLinkButtonSettings = new QCommandLinkButton(layoutWidgetButtons);
  QIcon icon2 = QIcon::fromTheme("settings");
  mCommandLinkButtonSettings->setIcon(icon2);
  mCommandLinkButtonSettings->setIconSize(QSize(48, 48));

  verticalLayout->addWidget(mCommandLinkButtonSettings);

  QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

  verticalLayout->addItem(verticalSpacer);

  mSplitter->addWidget(layoutWidgetButtons);

  QWidget *layoutWidget2 = new QWidget(mSplitter);
  QVBoxLayout *verticalLayout2 = new QVBoxLayout(layoutWidget2);
  verticalLayout2->setSpacing(6);
  verticalLayout2->setContentsMargins(11, 11, 11, 11);
  verticalLayout2->setContentsMargins(9, 9, 9, 9);
  mLabelRecentProjects = new QLabel(layoutWidget2);
  mLabelRecentProjects->setMinimumSize(QSize(0, 68));
  mLabelRecentProjects->setMaximumSize(QSize(16777215, 68));
  QFont font1;
  font1.setFamily(QStringLiteral("Segoe UI"));
  font1.setPointSize(12);
  mLabelRecentProjects->setFont(font1);
  mLabelRecentProjects->setStyleSheet(QStringLiteral("color: rgb(0, 0, 127);"));

  verticalLayout2->addWidget(mLabelRecentProjects);

  mListWidgetRecentProjects = new QListWidget(layoutWidget2);
  QFont font2;
  font2.setPointSize(11);
  mListWidgetRecentProjects->setFont(font2);
  mListWidgetRecentProjects->setFrameShape(QFrame::NoFrame);
  mListWidgetRecentProjects->setFrameShadow(QFrame::Sunken);
  mListWidgetRecentProjects->setViewMode(QListView::ListMode);

  verticalLayout2->addWidget(mListWidgetRecentProjects);

  mCommandLinkButtonClearHistory = new QCommandLinkButton(layoutWidget2);
  mCommandLinkButtonClearHistory->setIcon(QIcon::fromTheme("delete-history"));
  mCommandLinkButtonClearHistory->setIconSize(QSize(24, 24));

  verticalLayout2->addWidget(mCommandLinkButtonClearHistory);

  mSplitter->addWidget(layoutWidget2);

  layout->addWidget(mSplitter, 1, 0, 1, 1);

  StartPageWidget::retranslate();
}

void StartPageWidget::initSignalAndSlots()
{
  connect(mCommandLinkButtonNewProject,   &QAbstractButton::clicked,  this, &StartPageWidget::openNew);
  connect(mCommandLinkButtonOpenProject,  &QAbstractButton::clicked,  this, &StartPageWidget::openProject);
  connect(mCommandLinkButtonSettings,     &QAbstractButton::clicked,  this, &StartPageWidget::openSettings);
  connect(mCommandLinkButtonClearHistory, &QAbstractButton::clicked,  this, &StartPageWidget::clearHistory);

  connect(mListWidgetRecentProjects,      &QListWidget::currentTextChanged, this, &StartPageWidget::openProjectFromHistory);
}

void StartPageWidget::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  switch (event->type()) {
    case QEvent::LanguageChange:
      this->retranslate();
      break;
    default:
      break;
  }
}

} // namespace graphos
