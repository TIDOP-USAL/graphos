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


#ifndef GRAPHOS_CREATE_PROJECT_INTERFACES_H
#define GRAPHOS_CREATE_PROJECT_INTERFACES_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{


class CreateProjectModel
  : public Model
{

  Q_OBJECT

public:

  CreateProjectModel() {}

  ~CreateProjectModel() override = default;
  
  /*!
   * \brief Se crea un nuevo proyecto
   */
  virtual void create() = 0;

  /*!
   * \brief Se guarda el proyecto
   */
  virtual void save(const QString &file) = 0;

  /*!
   * \brief Establece el nombre del proyecto
   * \param name Nombre
   */
  virtual void setProjectName(const QString &name) = 0;

  /*!
   * \brief Establece la ruta del proyecto
   * \param path Ruta del proyecto
   */
  virtual void setProjectPath(const QString &path) = 0;

  /*!
   * \brief Establece la descripción del proyecto
   * \param description Descripción del proyecto
   */
  virtual void setProjectDescription(const QString &description) = 0;

  virtual void setDatabase(const QString &database) = 0;


// Model interface

private:

  void init() override
  {
  }

public slots:

  void clear() override
  {
  }
};



class CreateProjectView
  : public DialogView
{

  Q_OBJECT

public:

  CreateProjectView(QWidget *parent) : DialogView(parent) {}
  virtual ~CreateProjectView() override = default;

  /*!
   * \brief Devuelve el nombre del proyecto
   * \return Nombre del proyecto
   */
  virtual QString projectName() const = 0;

  /*!
   * \brief Devuelve la ruta del proyecto
   * \return Ruta del proyecto
   */
  virtual QString projectPath() const = 0;

  /*!
   * \brief Establece la ruta del proyecto
   * \param path Ruta del proyecto
   */
  virtual void setProjectPath(const QString &path) = 0;

  /*!
   * \brief Devuelve la descripción del proyecto
   * \return Descripción del proyecto
   */
  virtual QString projectDescription() const = 0;

  //virtual bool createProjectFolderEnable() const = 0;

//protected slots:

  //virtual void onClickButtonSelectPath() = 0;

};



class CreateProjectPresenter
  : public Presenter
{

  Q_OBJECT

public:

  CreateProjectPresenter(){}

  virtual ~CreateProjectPresenter() override = default;

signals:

  /*!
   * \brief Señal que se emite cuando se crea el proyecto
   */
  void projectCreated();

protected slots:

  /*!
   * \brief Guarda el proyecto
   */
  virtual void saveProject() = 0;

  /*!
   * \brief Descarta los cambios en el proyecto
   */
  virtual void discartProject() = 0;

};

} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_INTERFACES_H
