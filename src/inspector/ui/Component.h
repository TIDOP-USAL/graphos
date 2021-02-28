#ifndef INSPECTOR_COMPONENT_H
#define INSPECTOR_COMPONENT_H

#include <QObject>
#include <QString>

#include <tidop/core/flags.h>

class QAction;

namespace inspector
{

namespace ui
{

class IModel;
class IDialogView;
class IPresenter;


class Component
  : public QObject
{

  Q_OBJECT

public:

  /*!
   * /brief Component dependencies for activate
   */
  enum class Dependencies
  {
    none = (0 << 0),
    project = (1 << 0),
    images_loaded = (1 << 1),
    no_processing = (1 << 2),
    feature_extraction = (1 << 3),
    feature_matching = (1 << 4),
    oriented = (1 << 5),
    absolute_oriented = (1 << 6),
    dense_model = (1 << 7)
  };

public:

  Component() {}
  virtual ~Component() = default;

  virtual QString name() const = 0;
  virtual QAction *action() const = 0;
  virtual QString menu() const = 0;
  virtual QString toolbar() const = 0;
  virtual tl::EnumFlags<Dependencies> dependencies() const = 0;
  
  //virtual IModel *model() = 0;
  //virtual IDialogView *view() = 0;
  //virtual IPresenter *presenter() = 0;

protected:

  virtual void setName(const QString &name) = 0;
  //virtual void setAction(QAction *action) = 0;
  virtual void setMenu(const QString &menu) = 0;
  virtual void setToolbar(const QString &toolbar) = 0;
  virtual void setDependencies(const tl::EnumFlags<Dependencies> &dependencies) = 0;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Component::Dependencies)

class ComponentBase
  : public Component
{

  Q_OBJECT

public:

  ComponentBase();
  ~ComponentBase();

protected:

  virtual void init();
  virtual void createComponent();
  virtual void createAction();
  virtual void createModel() = 0;
  virtual void createView() = 0;
  virtual void createPresenter() = 0;

// Component

public:

  QString name() const override;
  QAction *action() const override;
  QString menu() const override;
  QString toolbar() const override;
  tl::EnumFlags<Dependencies> dependencies() const override;
  //IModel *model() override;
  //IDialogView *view() override;
  //IPresenter *presenter() override;

protected:

  void setName(const QString &name) override;
  //void setAction(QAction *action) override;
  void setMenu(const QString &menu) override;
  void setToolbar(const QString &toolbar) override;
  void setDependencies(const tl::EnumFlags<Dependencies> &dependencies) override;
  
protected:

  QString mName;
  QAction *mAction;
  IModel *mModel;
  IDialogView *mView;
  IPresenter *mPresenter;

private:

  QString mMenu;
  QString mToolbar;
  tl::EnumFlags<Dependencies> mDependencies;
};


class ProcessComponent
  : public ComponentBase
{

  Q_OBJECT

public:

  ProcessComponent() {}
  ~ProcessComponent() override = default;

signals:

  void running();
  void finished();
  void failed();


};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENT_H
