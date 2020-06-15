#ifndef INSPECTOR_FEATURE_MATCHING_INTERFACES_H
#define INSPECTOR_FEATURE_MATCHING_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/features/features.h"

#include <QString>

namespace inspector
{

namespace ui
{

class ProgressHandler;

class FeatureMatchingModel
  : public IModel
{

  Q_OBJECT

public:

  FeatureMatchingModel(QObject *parent = nullptr) : IModel(parent) {}
  ~FeatureMatchingModel() override = default;

  virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
  virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
  virtual QString database() const = 0;
  virtual void writeMatchPairs() = 0;
};


class FeatureMatchingView
  : public IDialogView
{

  Q_OBJECT

public:

  FeatureMatchingView(QWidget *parent) : IDialogView(parent) {}
  ~FeatureMatchingView() override = default;

  virtual void addMatchMethod(QWidget *matchMethod) = 0;
  virtual QString currentMatchMethod() const = 0;

signals:

  void matchMethodChange(QString);
  void run();

public slots:

  virtual void setCurrentMatchMethod(const QString &matchMethodName) = 0;

};


class FeatureMatchingPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  FeatureMatchingPresenter(){}
  ~FeatureMatchingPresenter() override = default;

signals:

  void running();
  void matchingFinished();
  void finished();

public slots:

  virtual void setProgressHandler(ProgressHandler *progressHandler) = 0;
  virtual void cancel() = 0;

private slots:

  virtual void run() = 0;
  virtual void setCurrentMatchMethod(const QString &matchMethod) = 0;

};


} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_INTERFACES_H
