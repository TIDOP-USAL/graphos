#ifndef INSPECTOR_ORIENTATION_FORMAT_BUNDLER_WIDGETS_H
#define INSPECTOR_ORIENTATION_FORMAT_BUNDLER_WIDGETS_H

#include "inspector/widgets/InspectorWidget.h"


class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace inspector
{

class INSPECTOR_EXPORT BundlerFormatWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  BundlerFormatWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~BundlerFormatWidget() override = default;

  virtual QString file() const = 0;

signals:

  void fileChanged(QString);

public slots:

  virtual void setFile(const QString &file) = 0;

};

class INSPECTOR_EXPORT BundlerFormatWidgetImp
  : public BundlerFormatWidget
{

  Q_OBJECT

public:

  BundlerFormatWidgetImp(QWidget *parent = nullptr);
  ~BundlerFormatWidgetImp() override = default;

protected slots:

  void onPushButtonSelectPath();

// IWidgetView interface

  QString file() const override;

signals:

  void fileChanged(const QString &);

public slots:

  void setFile(const QString &file) override;

// IWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGroupBox *mGroupBox;
  QLabel *mLabelFile;
  QLineEdit *mLineEditFile;
  QPushButton *mPushButtonSelectPath;

};

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_FORMAT_BUNDLER_WIDGETS_H
