/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CREATE_PROJECT_VIEW_H
#define GRAPHOS_CREATE_PROJECT_VIEW_H

#include "graphos/ui/createproject/CreateProjectView.h"

class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QTextEdit;
class QPushButton;
class QLabel;

namespace graphos
{

class CreateProjectViewImp
  : public CreateProjectView
{
  Q_OBJECT

public:

  explicit CreateProjectViewImp(QWidget *parent = nullptr);
  ~CreateProjectViewImp() override = default;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// CreateProjectView interface

public:

  QString projectName() const override;
  QString projectPath() const override;
  void setProjectPath(const QString &path) override;
  QString projectDescription() const override;
  bool createProjectFolderEnable() const override;
  void setExistingProject(bool prjExist) override;

protected slots:

  void onClickButtonSelectPath() override;

protected:

  QLabel *mLabelProjectName;
  QLineEdit *mLineEditProjectName;
  QLabel *mLabelProjectPath;
  QLineEdit *mLineEditProjectPath;
  QLabel *mLabelProjectFile;
  QLineEdit *mLineEditProjectFile;
  QLabel *mLabelDescription;
  QTextEdit *mTextEditDescription;
  QCheckBox *mCheckBoxProjectFolder;
  QPushButton *mPushButtonProjectPath;
  QDialogButtonBox *mButtonBox;
  bool bPrjExist;
};

} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_VIEW_H
