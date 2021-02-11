#ifndef INSPECTOR_ORIENTATION_INTERFACES_H
#define INSPECTOR_ORIENTATION_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/orientation/photoorientation.h"
#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{

class OrientationModel
  : public IModel
{

public:

  OrientationModel(QObject *parent = nullptr) : IModel(parent) {}
  ~OrientationModel() override = default;

  virtual bool refinePrincipalPoint() const = 0;
  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setSparseModel(const QString &sparseModel) = 0;
  virtual void setOffset(const QString &offset) = 0;
  virtual bool isPhotoOriented(const QString &imgName) const = 0;
  virtual PhotoOrientation photoOrientation(const QString &imgName) const = 0;
  virtual void addPhotoOrientation(const QString &imgName, const PhotoOrientation &orientation) = 0;
  virtual QString database() const = 0;
  virtual QString imagePath() const = 0;
  virtual QString projectPath() const = 0;
  virtual bool gpsOrientation() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual void setReconstructionPath(const QString &reconstructionPath) = 0;
  virtual std::map<QString, std::array<double, 3>> cameraPositions() const = 0;
};


class OrientationView
  : public IDialogView
{

  Q_OBJECT

public:

  OrientationView(QWidget *parent = nullptr) : IDialogView(parent) {}
  ~OrientationView() override = default;

  virtual bool refinePrincipalPoint() const = 0;
  virtual bool absoluteOrientation() const = 0;
  virtual bool isEnabledAbsoluteOrientation() const = 0;

public slots:

  virtual void setRefinePrincipalPoint(bool refine) = 0;
  virtual void setAbsoluteOrientation(bool active) = 0;
  virtual void enabledAbsoluteOrientation(bool enabled) = 0;

signals:

  void run();
  void refinePrincipalPoint(bool);
  void absoluteOrientationChange(bool);
  void enabledAbsoluteOrientationChange(bool);
};


class OrientationPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  OrientationPresenter() {}
  ~OrientationPresenter() override = default;

signals:

  void orientationFinished();

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_ORIENTATION_INTERFACES_H
