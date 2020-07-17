#ifndef INSPECTOR_ORIENTATION_INTERFACES_H
#define INSPECTOR_ORIENTATION_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/orientation/photoorientation.h"
namespace inspector
{

namespace ui
{

class ProgressHandler;

class OrientationModel
  : public IModel
{

public:

  OrientationModel(QObject *parent = nullptr) : IModel(parent) {}
  ~OrientationModel() override = default;

  virtual bool refinePrincipalPoint() const = 0;
  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual PhotoOrientation photoOrientation(const QString &imgName) const = 0;
  virtual void addPhotoOrientation(const QString &imgName, const PhotoOrientation &orientation) = 0;
  virtual QString database() const = 0;
  virtual QString imagePath() const = 0;
  virtual QString projectPath() const = 0;

};


class OrientationView
  : public IDialogView
{

  Q_OBJECT

public:

  OrientationView(QWidget *parent = nullptr) : IDialogView(parent) {}
  ~OrientationView() override = default;

  virtual bool refinePrincipalPoint() const = 0;

public slots:

  virtual void setRefinePrincipalPoint(bool refine) = 0;

signals:

  void run();
  void refinePrincipalPoint(bool);

public slots:

};


class OrientationPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  OrientationPresenter() {}
  ~OrientationPresenter() override = default;

signals:

  void running();
  void finished();
  void orientationFinished();

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler) = 0;
  virtual void cancel() = 0;

private slots:

  virtual void run() = 0;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_ORIENTATION_INTERFACES_H
