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

#ifndef GRAPHOS_CREATE_PROJECT_VIEW_H
#define GRAPHOS_CREATE_PROJECT_VIEW_H

#include "graphos/components/createproject/CreateProjectView.h"

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

protected slots:

    void onClickButtonSelectPath();

// CreateProjectView interface

public:

    auto projectName() const -> QString override;
    auto projectPath() const -> QString override;
    void setProjectPath(const QString &path) override;
    auto projectDescription() const -> QString override;
    auto createProjectFolderEnable() const -> bool override;
    void setExistingProject(bool prjExist) override;

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

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
    bool mProjectExist;
};

} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_VIEW_H
