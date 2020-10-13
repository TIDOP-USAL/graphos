#ifndef INSPECTOR_EXPORT_ORIENTATIONS_INTERFACES_H
#define INSPECTOR_EXPORT_ORIENTATIONS_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class ExportOrientationsModel
  : public IModel
{

  Q_OBJECT

public:

  ExportOrientationsModel(QObject *parent = nullptr) : IModel(parent) {}
  ~ExportOrientationsModel() override = default;

  virtual QString reconstruction() const = 0;

};


class ExportOrientationsView
  : public IDialogView
{

  Q_OBJECT

public:

  ExportOrientationsView(QWidget *parent) : IDialogView(parent) {}
  ~ExportOrientationsView() override = default;

  virtual void addFormatWidget(QWidget *formatWidget) = 0;
  virtual QString format() const = 0;

public slots:

  virtual void setCurrentFormat(const QString &format) = 0;

signals:

  void formatChange(const QString &);
};

class ExportOrientationsPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  ExportOrientationsPresenter(){}
  ~ExportOrientationsPresenter() override = default;

public slots:

  virtual void save() = 0;
  virtual void setCurrentFormat(const QString &format) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_EXPORT_ORIENTATIONS_INTERFACES_H
