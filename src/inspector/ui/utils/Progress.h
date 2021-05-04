#ifndef INSPECTOR_PROGRESS_H
#define INSPECTOR_PROGRESS_H

#include <QObject>

namespace inspector
{

namespace ui
{


class ProgressHandler
  : public QObject
{
  Q_OBJECT

public:

  ProgressHandler(QObject *parent = nullptr);
  ~ProgressHandler() override;

public:

  int min() const;
  int max() const;
  int value() const;

signals:

  void rangeChange(int, int);
  void valueChange(int);
  void initialized();
  void finished();
  void titleChange(QString);
  void descriptionChange(QString);
  void cancel();

public slots:

  void setRange(int min, int max);
  void setValue(int value);
  void next();
  void init();
  void finish();
  void setTitle(const QString &title);
  void setDescription(const QString &description);

protected:

  int mMin;
  int mMax;
  int mValue;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_PROGRESS_H
