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

#define BOOST_TEST_MODULE GRAPHOS create project view test

#include <boost/test/unit_test.hpp>

#include "graphos/components/createproject/impl/CreateProjectView.h"

#include <QtTest>
#include <QApplication>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;

BOOST_AUTO_TEST_SUITE(TestCreateProjectViewSuite)

QApplication app(boost::unit_test::framework::master_test_suite().argc,
                 boost::unit_test::framework::master_test_suite().argv);

class TestCreateProjectView
  : public CreateProjectViewImp
{

public:

  TestCreateProjectView()
    : CreateProjectViewImp()
  {

  }

  ~TestCreateProjectView() {}

  void setup() { }

  void teardown() { }

};

BOOST_FIXTURE_TEST_CASE(_clear, TestCreateProjectView)
{
    mLineEditProjectName->setText("test");
    mLineEditProjectPath->setText("C:/Users/Tido/Documents/photomatch/Projects");

    clear();

    BOOST_CHECK_EQUAL(projectName().toStdString(), "");
    BOOST_CHECK_EQUAL(projectPath().toStdString(), "");
    BOOST_CHECK_EQUAL(mLineEditProjectFile->text().toStdString(), "");
    BOOST_CHECK_EQUAL(projectDescription().toStdString(), "");
    BOOST_CHECK_EQUAL(true, this->createProjectFolderEnable());
}

BOOST_FIXTURE_TEST_CASE(default_constructor, TestCreateProjectView)
{
    CreateProjectViewImp newProjectView;
    BOOST_CHECK_EQUAL("", newProjectView.projectName().toStdString());
    BOOST_CHECK_EQUAL("", newProjectView.projectPath().toStdString());
    BOOST_CHECK_EQUAL("", newProjectView.projectDescription().toStdString());
    BOOST_CHECK_EQUAL(true, newProjectView.createProjectFolderEnable());
}

BOOST_FIXTURE_TEST_CASE(project_name, TestCreateProjectView)
{

    /// Simulación de entrada por teclado

    mLineEditProjectName->clear();
    QTest::keyClicks(mLineEditProjectName, "project");

    BOOST_CHECK_EQUAL(projectName().toStdString(), "project");
}

BOOST_FIXTURE_TEST_CASE(project_path, TestCreateProjectView)
{
    setProjectPath("C:/Users/Tido/Documents/photomatch/Projects");
    BOOST_CHECK_EQUAL("C:/Users/Tido/Documents/photomatch/Projects", projectPath().toStdString());

    setProjectPath("C:/Users/user1/Documents/photomatch/Projects");
    BOOST_CHECK_EQUAL("C:/Users/user1/Documents/photomatch/Projects", projectPath().toStdString());
}

BOOST_FIXTURE_TEST_CASE(project_path_signals, TestCreateProjectView)
{
    /// Simulación de entrada por teclado

    QString project_path = projectPath();

    QTest::keyClick(mLineEditProjectPath, '_');
    BOOST_CHECK_EQUAL(projectPath().toStdString(), project_path.append("_").toStdString());

    mLineEditProjectPath->clear();
    QTest::keyClicks(mLineEditProjectPath, "C:/Users/user1/Documents/photomatch/Projects");
    BOOST_CHECK_EQUAL(projectPath().toStdString(), "C:/Users/user1/Documents/photomatch/Projects");
}

BOOST_FIXTURE_TEST_CASE(line_edit_project_file, TestCreateProjectView)
{
    /// Desactivado
    BOOST_CHECK_EQUAL(mLineEditProjectFile->isEnabled(), false);


    mLineEditProjectPath->clear();
    QTest::keyClicks(mLineEditProjectPath, "C:/Users/user1/Documents/photomatch/Projects");

    mLineEditProjectName->clear();
    QTest::keyClicks(mLineEditProjectName, "project_name");

    QTest::mouseClick(mCheckBoxProjectFolder, Qt::MouseButton::LeftButton);
    QString file(projectPath());
    if (createProjectFolderEnable())
        file.append(QDir::separator()).append(projectName());
    file.append(QDir::separator()).append(projectName()).append(".xml");
    BOOST_CHECK_EQUAL(mLineEditProjectFile->text().toStdString(), QDir::cleanPath(file).toStdString());


    QTest::mouseClick(mCheckBoxProjectFolder, Qt::MouseButton::LeftButton);
    file = projectPath();
    if (createProjectFolderEnable())
        file.append(QDir::separator()).append(projectName());
    file.append(QDir::separator()).append(projectName()).append(".xml");
    BOOST_CHECK_EQUAL(mLineEditProjectFile->text().toStdString(), QDir::cleanPath(file).toStdString());

    mLineEditProjectName->clear();
    BOOST_CHECK_EQUAL(mLineEditProjectFile->text().toStdString(), "");
}

BOOST_FIXTURE_TEST_CASE(project_description, TestCreateProjectView)
{
  /// Simulación de entrada por teclado

  QString project_description = projectDescription();

  QTest::keyClick(mTextEditDescription, '_');
  BOOST_CHECK_EQUAL(project_description.append("_").toStdString(), projectDescription().toStdString());

  mTextEditDescription->clear();

  QTest::keyClicks(mTextEditDescription, "description");
  BOOST_CHECK_EQUAL("description", projectDescription().toStdString());
}

BOOST_FIXTURE_TEST_CASE(create_project_folder, TestCreateProjectView)
{
  mCheckBoxProjectFolder->setChecked(true);
  BOOST_CHECK(createProjectFolderEnable());

  mCheckBoxProjectFolder->setChecked(false);
  BOOST_CHECK(!createProjectFolderEnable());
}

BOOST_FIXTURE_TEST_CASE(dialog_buttonbox, TestCreateProjectView)
{
  QSignalSpy spy_rejected(this, &CreateProjectViewImp::rejected);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Cancel), Qt::LeftButton);
  BOOST_CHECK_EQUAL(spy_rejected.count(), 1);

  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(true);
  QSignalSpy spy_accepted(this, &CreateProjectViewImp::accepted);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Save), Qt::LeftButton);
  BOOST_CHECK_EQUAL(spy_accepted.count(), 1);

  QSignalSpy spy_help(this, &CreateProjectViewImp::help);
  QTest::mouseClick(mButtonBox->button(QDialogButtonBox::Help), Qt::LeftButton);
  BOOST_CHECK_EQUAL(spy_help.count(), 1);
}

BOOST_FIXTURE_TEST_CASE(set_existing_project, TestCreateProjectView)
{
  mLineEditProjectName->setText("Prueba");
  setExistingProject(true);
  QPalette palette = mLineEditProjectName->palette();
  QColor color = palette.color(QPalette::Text);
  BOOST_CHECK(QColor(Qt::red) == color);
  //BOOST_CHECK_EQUAL(false, mButtonBox->button(QDialogButtonBox::Save)->isEnabled());

  setExistingProject(false);
  palette = mLineEditProjectName->palette();
  color = palette.color(QPalette::Text);
  BOOST_CHECK(QColor(Qt::black) == color);
  //BOOST_CHECK_EQUAL(true, mButtonBox->button(QDialogButtonBox::Save)->isEnabled());
}


BOOST_AUTO_TEST_SUITE_END()