#ifndef INSPECTOR_PROCESS_PRESENTER_H
#define INSPECTOR_PROCESS_PRESENTER_H

#include <QObject>

#include "inspector/interfaces/mvp.h"

class MultiProcess;

namespace inspector
{

namespace ui
{

class ProgressHandler;

class ProcessPresenter
  : public Presenter
{

  Q_OBJECT

public:

  ProcessPresenter();
  ~ProcessPresenter() override;

protected slots:

  virtual void onError(int code, const QString &msg);
  virtual void onFinished();

  /*!
   * \brief Crea y configura el proceso
   * return True si se se a creado correctamente
   */
  virtual bool createProcess() = 0;

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler);
  virtual void run();
  virtual void cancel();

signals:

  void running();
  void finished();
  void failed();

protected:

  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;
};


} // namespace ui

} // namespace inspector


#endif // INSPECTOR_PROCESS_PRESENTER_H
