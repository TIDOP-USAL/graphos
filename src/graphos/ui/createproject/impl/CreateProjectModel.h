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

#ifndef GRAPHOS_CREATE_PROJECT_MODEL_H
#define GRAPHOS_CREATE_PROJECT_MODEL_H

#include "graphos/ui/createproject/CreateProjectModel.h"

namespace graphos
{

class Project;


class CreateProjectModelImp
  : public CreateProjectModel
{

  Q_OBJECT

public:

  explicit CreateProjectModelImp(Project *project,
                                 QObject *parent = nullptr);
  ~CreateProjectModelImp() override;

signals:



// CreateProjectModel interface

public:

  QString projectName() const override;
  QString projectDescription() const override;
  QString projectFolder() const override;
  QString projectPath() const override;

public slots:

  void setProjectName(const QString &name) override;
  void setProjectDescription(const QString &projectDescription) override;
  void setProjectFolder(const QString &dir) override;
  void setDatabase(const QString &database) override;
  void saveAs(const QString &file) override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QString mPrjFile;
  bool bUnsavedChanges;

};

} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_MODEL_H
