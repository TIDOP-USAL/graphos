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

#include "CreateProjectView.h"

/* Qt */
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace graphos
{
	
CreateProjectViewImp::CreateProjectViewImp(QWidget *parent)
  : CreateProjectView(parent),
    mLabelProjectName(new QLabel(this)),
    mLineEditProjectName(new QLineEdit(this)),
    mLabelProjectPath(new QLabel(this)),
    mLineEditProjectPath(new QLineEdit(this)),
    mLabelProjectFile(new QLabel(this)),
    mLineEditProjectFile(new QLineEdit(this)),
    mLabelDescription(new QLabel(this)),
    mTextEditDescription(new QTextEdit(this)),
    mCheckBoxProjectFolder(new QCheckBox(this)),
    mPushButtonProjectPath(new QPushButton(this)),
    mButtonBox(new QDialogButtonBox(this)),
    bPrjExist(false)
{
  initUI();
  initSignalAndSlots();
}

void CreateProjectViewImp::initUI()
{
  setObjectName(QString("CreateProjectView"));
  resize(450,300);

  QGridLayout *layout = new QGridLayout();
  setLayout(layout);

  layout->addWidget(mLabelProjectName, 0, 0);
  layout->addWidget(mLineEditProjectName, 0, 1, 1, 2);

  layout->addWidget(mLabelProjectPath, 1, 0);
  layout->addWidget(mLineEditProjectPath, 1, 1);
  mPushButtonProjectPath->setMaximumSize(QSize(31,28));
  mPushButtonProjectPath->setText("...");
  layout->addWidget(mPushButtonProjectPath, 1, 2);

  layout->addWidget(mLabelProjectFile, 2, 0);
  mLineEditProjectFile->setEnabled(false);
  layout->addWidget(mLineEditProjectFile, 2, 1, 1, 2);

  mCheckBoxProjectFolder->setChecked(true);
  layout->addWidget(mCheckBoxProjectFolder, 3, 0, 1, 3);

  layout->addWidget(mLabelDescription, 4, 0);

  layout->addWidget(mTextEditDescription, 5, 0, 1, 3);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  layout->addWidget(mButtonBox, 7, 0, 1, 3);

  retranslate();
  update();
}

void CreateProjectViewImp::initSignalAndSlots()
{
  connect(mLineEditProjectName,   &QLineEdit::textChanged,   this, &CreateProjectViewImp::update);
  connect(mLineEditProjectName,   &QLineEdit::textChanged,   this, &CreateProjectView::project_name_changed);
  connect(mLineEditProjectPath,   &QLineEdit::textChanged,   this, &CreateProjectViewImp::update);
  connect(mCheckBoxProjectFolder, &QCheckBox::stateChanged,  this, &CreateProjectViewImp::update);
  connect(mCheckBoxProjectFolder, &QCheckBox::stateChanged,  this, &CreateProjectView::project_name_changed);
  connect(mPushButtonProjectPath, &QAbstractButton::clicked, this, &CreateProjectViewImp::onClickButtonSelectPath);

  connect(mButtonBox,  &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(mButtonBox,  &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Help),     &QAbstractButton::clicked, this, &DialogView::help);
}

void CreateProjectViewImp::clear()
{
  mLineEditProjectName->clear();
  mLineEditProjectPath->clear();
  mLineEditProjectFile->clear();
  mTextEditDescription->clear();
  mCheckBoxProjectFolder->setChecked(true);
}

void CreateProjectViewImp::update()
{
  bool bSave = !mLineEditProjectName->text().isEmpty() &&
               !mLineEditProjectPath->text().isEmpty();
  mButtonBox->button(QDialogButtonBox::Save)->setEnabled(bSave && !bPrjExist);

  if (bSave){
    QString file(mLineEditProjectPath->text());
    if (mCheckBoxProjectFolder->isChecked()){
      file.append(QDir::separator()).append(mLineEditProjectName->text());
    }
    file.append(QDir::separator()).append(mLineEditProjectName->text()).append(".xml");
    mLineEditProjectFile->setText(QDir::cleanPath(file));
  } else
    mLineEditProjectFile->setText("");
}

void CreateProjectViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("CreateProjectView", "New Project", nullptr));
  mLabelProjectName->setText(QApplication::translate("CreateProjectView", "Project Name", nullptr));
  mLabelProjectPath->setText(QApplication::translate("CreateProjectView", "Project Path", nullptr));
  mLabelProjectFile->setText(QApplication::translate("CreateProjectView", "Project File", nullptr));
  mCheckBoxProjectFolder->setText(QApplication::translate("CreateProjectView", "Create folder with project name", nullptr));
  mLabelDescription->setText(QApplication::translate("CreateProjectView", "Description", nullptr));

  mButtonBox->button(QDialogButtonBox::Save)->setText(QApplication::translate("CreateProjectView", "Save", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("CreateProjectView", "Cancel", nullptr));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("CreateProjectView", "Help", nullptr));
}

QString CreateProjectViewImp::projectName() const
{
  return mLineEditProjectName->text();
}

QString CreateProjectViewImp::projectPath() const
{
  return mLineEditProjectPath->text();
}

void CreateProjectViewImp::setProjectPath(const QString &path)
{
  mLineEditProjectPath->setText(path);
}

QString CreateProjectViewImp::projectDescription() const
{
  return mTextEditDescription->toPlainText();
}

bool CreateProjectViewImp::createProjectFolderEnable() const
{
  return mCheckBoxProjectFolder->isChecked();
}

void CreateProjectViewImp::setExistingProject(bool prjExist)
{
  bPrjExist = prjExist;
  QPalette palette;
  if (bPrjExist){
    palette.setColor(QPalette::Text, Qt::red);
  } else {
    palette.setColor(QPalette::Text, Qt::black);
  }
  mLineEditProjectName->setPalette(palette);
  
  update();
}

void CreateProjectViewImp::onClickButtonSelectPath()
{
  QString pathName = QFileDialog::getExistingDirectory(this,
    tr("Project path"),
    mLineEditProjectPath->text(),
    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if (!pathName.isEmpty()) {
    mLineEditProjectPath->setText(pathName);
  }
}

} // namespace graphos
