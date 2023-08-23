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

#include "AboutView.h"

#include <QApplication>
#include <QIcon>
#include <QGridLayout>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QStackedWidget>
#include <QPlainTextEdit>
#include <QDialogButtonBox>

namespace graphos
{

AboutViewImp::AboutViewImp(QWidget *parent)
  : AboutView(parent)
{
    AboutViewImp::initUI();
    AboutViewImp::initSignalAndSlots();
}

AboutViewImp::~AboutViewImp()
{
}

void AboutViewImp::setGraphosVersion(const QString &version)
{
    mLabelGraphosVersion->setText(mLabelGraphosVersion->text() + " " + version);
}

void AboutViewImp::setGraphosLicence(const QString &licence)
{
    mTextEditGraphosLicence->setPlainText(licence);
}

void AboutViewImp::addLicence(const QString &product,
                              const QString &licence)
{
    QListWidgetItem *item = new QListWidgetItem(mListWidgetThirdPartyLiceses);
    item->setText(product);

    QWidget *page = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(page);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    QPlainTextEdit *plainText = new QPlainTextEdit(page);
    plainText->setPlainText(licence);
    gridLayout->addWidget(plainText, 0, 0, 1, 1);

    mStackedWidgetThirdPartyLicenses->addWidget(page);
}

void AboutViewImp::initUI()
{
    this->setObjectName(QString::fromUtf8("AboutView"));
    this->resize(692, 540);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    mLabelGraphosLogo = new QLabel(this);
    mLabelGraphosLogo->setPixmap(QPixmap(QString::fromUtf8(":/images/img/graphos.png")));
    layout->addWidget(mLabelGraphosLogo, 0, 0, 1, 2);

    mTabWidget = new QTabWidget(this);

    mTabGraphosLicence = new QWidget();
    mTabGraphosLicence->setObjectName(QString::fromUtf8("tab"));
    gridLayout_4 = new QGridLayout(mTabGraphosLicence);
    gridLayout_4->setSpacing(0);
    gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
    gridLayout_4->setContentsMargins(9, 3, 9, 3);

    mTextEditGraphosLicence = new QTextEdit(mTabGraphosLicence);
    gridLayout_4->addWidget(mTextEditGraphosLicence, 1, 0, 1, 1);

    mTabWidget->addTab(mTabGraphosLicence, QString());


    mTabThirdPartyLiceses = new QWidget();

    gridLayout_3 = new QGridLayout(mTabThirdPartyLiceses);
    mListWidgetThirdPartyLiceses = new QListWidget(mTabThirdPartyLiceses);
    mListWidgetThirdPartyLiceses->setMaximumSize(QSize(16777215, 120));
    mListWidgetThirdPartyLiceses->setSortingEnabled(false);

    gridLayout_3->addWidget(mListWidgetThirdPartyLiceses, 0, 0, 1, 1);

    mStackedWidgetThirdPartyLicenses = new QStackedWidget(mTabThirdPartyLiceses);

    gridLayout_3->addWidget(mStackedWidgetThirdPartyLicenses, 1, 0, 1, 1);

    mTabWidget->addTab(mTabThirdPartyLiceses, QString());

    layout->addWidget(mTabWidget, 3, 0, 1, 3);

    mLabelGraphosVersion = new QLabel(this);
    mLabelGraphosVersion->setObjectName(QString::fromUtf8("label_2"));

    layout->addWidget(mLabelGraphosVersion, 1, 0, 1, 3);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setObjectName(QString::fromUtf8("buttonBox"));
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Ok);

    layout->addWidget(mButtonBox, 4, 0, 1, 3);

    mTabWidget->setCurrentIndex(0);
    mStackedWidgetThirdPartyLicenses->setCurrentIndex(0);

    this->retranslate();
}

void AboutViewImp::initSignalAndSlots()
{
    connect(mListWidgetThirdPartyLiceses, &QListWidget::currentRowChanged,
            mStackedWidgetThirdPartyLicenses, &QStackedWidget::setCurrentIndex);
    connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::close);
}

void AboutViewImp::clear()
{

}

void AboutViewImp::update()
{

}

void AboutViewImp::retranslate()
{
    this->setWindowTitle(QCoreApplication::translate("AboutView", "About Graphos", nullptr));
    mTabWidget->setTabText(mTabWidget->indexOf(mTabGraphosLicence), QCoreApplication::translate("AboutView", "Licence", nullptr));
    mTabWidget->setTabText(mTabWidget->indexOf(mTabThirdPartyLiceses), QCoreApplication::translate("AboutView", "Third Party Licences", nullptr));
    mLabelGraphosVersion->setText(QCoreApplication::translate("AboutView", "Version:", nullptr));
}


} // namespace graphos
