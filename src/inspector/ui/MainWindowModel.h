#ifndef INSPECTOR_MAINWINDOW_MODEL_H
#define INSPECTOR_MAINWINDOW_MODEL_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class MainWindowModel
  : public IModel
{
  Q_OBJECT

public:

  explicit MainWindowModel();

  /*!
   * \brief Devuelve la ruta por defecto donde se situan los proyectos
   * \return
   */
  //QString defaultPath() const;

signals:

public slots:


// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  QString mPrjDefaultPath;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MAINWINDOW_MODEL_H
