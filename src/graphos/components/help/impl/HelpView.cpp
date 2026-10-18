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


#include "HelpView.h"

#include "graphos/core/Settings.h"
#include "graphos/framework/app/Application.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QHash>
#include <QStandardPaths>
#include <QTextDocument>
#include <QToolBar>
#include <QHelpLink>

using namespace tl;

namespace graphos
{


HelpBrowser::HelpBrowser(QHelpEngine *helpEngine, QWidget *parent)
  : QTextBrowser(parent),
    mHelpEngine(helpEngine)
{
    this->setStyleSheet("QTextBrowser { background-color : rgb(255, 255, 255); }");

    const QStringList documentationNamespaces = mHelpEngine->registeredDocumentations();
    if (documentationNamespaces.isEmpty()) {
        Message::error("No GRAPHOS documentation is registered in the Qt Help collection.");
        return;
    }

    const QUrl stylesheetUrl(QString("qthelp://%1/doc/css/styles.css")
                                 .arg(documentationNamespaces.constFirst()));
    const QByteArray stylesheet = mHelpEngine->fileData(stylesheetUrl);
    if (stylesheet.isEmpty()) {
        Message::error("Could not load GRAPHOS help stylesheet: {}", stylesheetUrl.toString().toStdString());
        return;
    }

    document()->setDefaultStyleSheet(QString::fromUtf8(stylesheet));
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    if (name.scheme() == "qthelp")
        return QVariant(mHelpEngine->fileData(name));

    return QTextBrowser::loadResource(type, name);
}

void HelpBrowser::setDocumentActivated(const QHelpLink &link, const QString &s)
{
    this->setSource(link.url);
}





HelpViewImp::HelpViewImp(QWidget *parent)
  : HelpView(parent)
{
    HelpViewImp::initUI();
    HelpViewImp::initSignalAndSlots();
    this->setModal(true);
}

HelpViewImp::~HelpViewImp() = default;

void HelpViewImp::initUI()
{
    const QString language = Application::instance().settings()->language();
    const QString helpLanguage = language.startsWith("es", Qt::CaseInsensitive)
        ? QString("es")
        : QString("en");
    const QString helpDirectory = QDir(QApplication::applicationDirPath())
        .filePath(QString("help/%1").arg(helpLanguage));
    const QString helpArchive = QDir(helpDirectory).filePath("graphos.qch");
    const QString helpCollectionTemplate = QDir(helpDirectory).filePath("graphos.qhc");

    QString collectionDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (collectionDirectory.isEmpty()) {
        Message::error("Could not locate the application data directory for the help collection.");
        collectionDirectory = QApplication::applicationDirPath();
    }
    QDir collectionRoot(collectionDirectory);
    if (!collectionRoot.mkpath(helpLanguage))
        Message::error("Could not create the help collection directory: {}", collectionDirectory.toStdString());
    const QString collectionPath = collectionRoot.filePath(QString("%1/graphos.qhc").arg(helpLanguage));
    const QFileInfo collectionInfo(collectionPath);
    if (!collectionInfo.exists() || collectionInfo.size() == 0) {
        if (collectionInfo.exists() && !QFile::remove(collectionPath)) {
            Message::error("Could not remove the empty help collection: {}", collectionPath.toStdString());
        } else if (!QFile::copy(helpCollectionTemplate, collectionPath)) {
            Message::error("Could not initialize the GRAPHOS help collection from '{}': {}",
                           helpCollectionTemplate.toStdString(),
                           collectionPath.toStdString());
        }
    }

    mHelpEngine = new QHelpEngine(collectionPath, this);

    if (!QFileInfo::exists(helpArchive)) {
        Message::error("GRAPHOS help archive not found: {}", helpArchive.toStdString());
    }

    if (!mHelpEngine->setupData()) {
        Message::error("Could not initialize the GRAPHOS help collection '{}': {}",
                       collectionPath.toStdString(),
                       mHelpEngine->error().toStdString());
    } else if (QFileInfo::exists(helpArchive)) {
        const QString helpNamespace = QHelpEngineCore::namespaceName(helpArchive);
        const bool isRegistered = mHelpEngine->registeredDocumentations().contains(helpNamespace);
        const QString registeredArchive = isRegistered
            ? QFileInfo(mHelpEngine->documentationFileName(helpNamespace)).canonicalFilePath()
            : QString();
        const QString currentArchive = QFileInfo(helpArchive).canonicalFilePath();
        const bool needsRegistration = !isRegistered ||
            registeredArchive.compare(currentArchive, Qt::CaseInsensitive) != 0;

        if (needsRegistration) {
            if (isRegistered && !mHelpEngine->unregisterDocumentation(helpNamespace)) {
                Message::error("Could not unregister the previous GRAPHOS help documentation '{}': {}",
                               helpNamespace.toStdString(),
                               mHelpEngine->error().toStdString());
            } else if (!mHelpEngine->registerDocumentation(helpArchive)) {
                Message::error("Could not register GRAPHOS help archive '{}' in collection '{}': {}",
                               helpArchive.toStdString(),
                               collectionPath.toStdString(),
                               mHelpEngine->error().toStdString());
            }
        }
    }

    mHelpBrowser = new HelpBrowser(mHelpEngine, this);
    mHelpBrowser->setMinimumSize(600, 450);

    QHelpContentWidget *content_widget = mHelpEngine->contentWidget();
    content_widget->setFrameStyle(QFrame::NoFrame);

    QTabWidget *tab_widget = new QTabWidget(this);
    tab_widget->addTab(content_widget, tr("Contents"));
    tab_widget->setMaximumWidth(300);
    tab_widget->setMinimumWidth(200);

    QWidget *search_tab_widget = new QWidget(this);
    QVBoxLayout *search_tab_layout = new QVBoxLayout(search_tab_widget);
    search_tab_layout->addWidget(mHelpEngine->searchEngine()->queryWidget());
    search_tab_layout->addWidget(new QLabel(tr("Search results: "), search_tab_widget));
    mSearchResultsWidget = new QTextBrowser(search_tab_widget);
    search_tab_layout->addWidget(mSearchResultsWidget);

    tab_widget->addTab(search_tab_widget, tr("Search"));
    tab_widget->setContentsMargins(0, 0, 0, 0);

    QToolBar *tool_bar = new QToolBar(this);

    mNavigateHomeAction = new QAction(this);
    mNavigateHomeAction->setIcon(QIcon::fromTheme("home"));
    connect(mNavigateHomeAction, SIGNAL(triggered()), this, SLOT(navigateHome()));
    tool_bar->addAction(mNavigateHomeAction);

    tool_bar->addSeparator();

    mNavigateBackwardAction = new QAction(this);
    mNavigateBackwardAction->setIcon(QIcon::fromTheme("back-button"));
    connect(mNavigateBackwardAction, SIGNAL(triggered()), this, SLOT(navigateBackward()));
    tool_bar->addAction(mNavigateBackwardAction);

    mNavigateForwardAction = new QAction(this);
    mNavigateForwardAction->setIcon(QIcon::fromTheme("forward-button"));
    connect(mNavigateForwardAction, SIGNAL(triggered()), this, SLOT(navigateForward()));
    tool_bar->addAction(mNavigateForwardAction);

    QWidget *help_widget = new QWidget(this);

    QHBoxLayout *hbox_layout = new QHBoxLayout(help_widget);
    hbox_layout->addWidget(tab_widget);
    hbox_layout->addWidget(mHelpBrowser);
    hbox_layout->setContentsMargins(0, 0, 0, 0);

    QGridLayout *grid_layout = new QGridLayout(this);
    grid_layout->setContentsMargins(0, 0, 0, 0);
    grid_layout->addWidget(tool_bar, 0, 0);
    grid_layout->addWidget(help_widget, 1, 0);
    grid_layout->setRowStretch(1, 1);
    resize(1100, 700);

    mHelpEngine->searchEngine()->reindexDocumentation();

    mSearchResultsWidget->setOpenLinks(false);
    mSearchResultsWidget->setOpenExternalLinks(false);

    retranslate();

}

void HelpViewImp::initSignalAndSlots()
{
    connect(mHelpBrowser, SIGNAL(forwardAvailable(bool)), mNavigateForwardAction, SLOT(setEnabled(bool)));
    connect(mHelpBrowser, SIGNAL(backwardAvailable(bool)), mNavigateBackwardAction, SLOT(setEnabled(bool)));

    connect(mHelpEngine->contentWidget(), &QHelpContentWidget::linkActivated,
            [&](const QUrl &name) {
                mHelpBrowser->setSource(name);
            });

    connect(mHelpEngine->indexWidget(), &QHelpIndexWidget::documentActivated,
            mHelpBrowser, [this](const QHelpLink &document, const QString &) {
                mHelpBrowser->setSource(document.url);
            });

    connect(mHelpEngine->searchEngine()->queryWidget(), SIGNAL(search()), this, SLOT(searchInvoked()));
    connect(mHelpEngine->searchEngine(), SIGNAL(searchingFinished(int)), this, SLOT(searchFinished(int)));

    connect(mSearchResultsWidget, SIGNAL(anchorClicked(const QUrl &)), mHelpBrowser, SLOT(setSource(const QUrl &)));
}

void HelpViewImp::retranslate()
{
    mNavigateHomeAction->setText(QApplication::translate("HelpDialog", "Home"));
    mNavigateHomeAction->setStatusTip(QApplication::translate("HelpDialog", "Home"));
    mNavigateBackwardAction->setText(QApplication::translate("HelpDialog", "Back"));
    mNavigateBackwardAction->setStatusTip(QApplication::translate("HelpDialog", "Back"));
    mNavigateForwardAction->setText(QApplication::translate("HelpDialog", "Forward"));
    mNavigateForwardAction->setStatusTip(QApplication::translate("HelpDialog", "Forward"));
}

void HelpViewImp::clear()
{

}

void HelpViewImp::update()
{
}

void HelpViewImp::navigateHome()
{
    mHelpEngine->contentWidget()->setCurrentIndex(mHelpEngine->contentWidget()->indexOf(mHomePage));
    mHelpBrowser->setSource(mHomePage);
}

void HelpViewImp::navigateForward()
{
    mHelpBrowser->forward();
}

void HelpViewImp::setUrlBase(const QString &urlBase)
{
    mUrlBase = urlBase;
}

void HelpViewImp::setHomePage(const QString &homePage)
{
    mHomePage = QUrl(homePage);
}

void HelpViewImp::navigateBackward()
{
    mHelpBrowser->backward();
}

void HelpViewImp::setHelpPage(const QString &helpPage)
{
    try {

        static const QHash<QString, QString> pageAliases{
            {"Help.html", "index.html"},
            {"menus.html", "interface.html"},
            {"menus.html#new_project", "workflow.html#project-setup"},
            {"menus.html#about", "index.html"},
            {"feature_extraction.html", "workflow.html#extract-features"},
            {"feature_matching.html", "workflow.html#match-features"},
            {"orientation.html", "workflow.html#estimate-orientation"},
            {"densification.html", "workflow.html#dense-products"},
            {"dense_cloud.html", "workflow.html#dense-products"},
            {"dtm.html", "tools.html#dem"},
            {"mesh.html", "tools.html#mesh"},
            {"orthophoto.html", "tools.html#orthophoto"},
            {"cameras.html", "tools.html#cameras"},
            {"matches_viewer.html", "tools.html#viewers"},
            {"keypoints_viewer.html", "tools.html#viewers"},
            {"load_from_video.html", "tools.html#images"},
            {"settings.html", "tools.html#settings"},
            {"Scale.html", "tools.html#gcps"},
            {"CoordinateReferenceSystem.html", "tools.html#crs"},
            {"export_cameras.html", "tools.html#export"},
            {"export_point_cloud.html", "tools.html#export"}
        };
        const QString page = pageAliases.value(helpPage, helpPage);
        const QString pageFile = page.section('#', 0, 0);
        if (pageFile != "index.html" &&
            pageFile != "workflow.html" &&
            pageFile != "tools.html" &&
            pageFile != "interface.html" &&
            pageFile != "commands.html") {
            navigateHome();
            return;
        }

        const QUrl url(QString(mUrlBase).append(page));
        QModelIndex indexOfPage = mHelpEngine->contentWidget()->indexOf(url);
        mHelpEngine->contentWidget()->setCurrentIndex(indexOfPage);
        mHelpBrowser->setSource(url);

    } catch (const std::exception &e) {
        tl::printException(e);
        this->navigateHome();
    }
}

void HelpViewImp::searchInvoked()
{
    mHelpEngine->searchEngine()->search(mHelpEngine->searchEngine()->queryWidget()->searchInput());
}

void HelpViewImp::searchFinished(int hits)
{
    auto results = mHelpEngine->searchEngine()->searchResults(0, hits);
    QString html = "<html><body>";
    for (auto &result : results) {
        html += "<a href=\"" + result.url().toString() + "\">" + result.title() + "</a><br>";
    }
    //QList<QPair<QString, QString> > hitList = mHelpEngine->searchEngine()->hits(0, hits - 1);
    //QString html = "<html><body>";
    //for (auto &hit : hitList) {
    //    QString url = hit.first;
    //    QString title = hit.second;
    //    html += "<a href=\"" + url + "\">" + title + "</a><br>";
    //}
    html += "</body></html>";
    mSearchResultsWidget->setHtml(html);
}

} // namespace graphos
