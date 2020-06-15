#ifndef INSPECTOR_DENSE_INTERFACES_H
#define INSPECTOR_DENSE_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/densification/densification.h"

#include <tidop/core/process.h>

#include <QString>

namespace inspector
{


class DensificationProcess;

namespace ui
{

class ProgressHandler;

class DensificationModel
  : public IModel
{

  Q_OBJECT

public:

  DensificationModel(QObject *parent = nullptr) : IModel(parent) {}
  ~DensificationModel() override = default;

  virtual std::shared_ptr<Densification> densification() const = 0;
  virtual QString projectFolder() const = 0;
  virtual QString imageDirectory() const = 0;

public slots:

  virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;
};



class DensificationView
  : public IDialogView
{

  Q_OBJECT


public:

  DensificationView(QWidget *parent) : IDialogView(parent) {}
  virtual ~DensificationView() {}

  virtual QString currentDensificationMethod() const = 0;

signals:

  void densificationChanged(QString);
  void run();

public slots:

  virtual void addDensification(QWidget *densification) = 0;
  virtual void setCurrentDensificationMethod(const QString &densificationMethod) = 0;

};


class DensificationPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  DensificationPresenter(){}
  ~DensificationPresenter() override = default;

signals:

  void running();
  void densificationFinished();
  void finished();

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler) = 0;
  virtual void cancel() = 0;

private slots:

  virtual void run() = 0;
  virtual void setCurrentDensifier(const QString &desnsifier) = 0;

};


} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_DENSE_INTERFACES_H
