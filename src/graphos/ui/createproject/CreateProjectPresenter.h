#ifndef GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H
#define GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H

#include <memory>

#include <QObject>

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class HelpDialog;

namespace ui
{
	
/*!
 * \brief The CreateProjectPresenter class
 */
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

} // namespace graphos


#endif // GRAPHOS_CREATE_PROJECT_PRESENTER_INTERFACE_H
