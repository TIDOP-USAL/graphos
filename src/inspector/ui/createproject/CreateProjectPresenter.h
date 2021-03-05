#ifndef INSPECTOR_CREATE_PROJECT_PRESENTER_INTERFACE_H
#define INSPECTOR_CREATE_PROJECT_PRESENTER_INTERFACE_H

#include <memory>

#include <QObject>

#include "inspector/interfaces/mvp.h"

namespace inspector
{

class HelpDialog;

namespace ui
{
	
/*!
 * \brief The CreateProjectPresenter class
 */
class CreateProjectPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  CreateProjectPresenter(){}

  virtual ~CreateProjectPresenter() override = default;

signals:

  /*!
   * \brief Señal que se emite cuando se crea el proyecto
   */
  void projectCreate();

protected slots:

  /*!
   * \brief Guarda el proyecto
   */
  virtual void saveProject() = 0;

  /*!
   * \brief Descarta los cambios en el proyecto
   */
  virtual void discartProject() = 0;

  /*!
   * \brief Comprueba si ya existe un proyecto con el mismo nombre
   */
  virtual void checkProjectName() const = 0;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_CREATE_PROJECT_PRESENTER_INTERFACE_H
