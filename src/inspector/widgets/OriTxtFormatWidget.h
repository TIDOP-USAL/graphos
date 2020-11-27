#ifndef INSPECTOR_ORIENTATION_FORMAT_TXT_WIDGET_H
#define INSPECTOR_ORIENTATION_FORMAT_TXT_WIDGET_H

#include "inspector/widgets/InspectorWidget.h"


class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;

namespace inspector
{

class INSPECTOR_EXPORT OriTxtFormatWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  OriTxtFormatWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~OriTxtFormatWidget() override = default;

  virtual QString file() const = 0;
  virtual QString rotation() const = 0;

signals:

  void fileChanged(QString);
  void rotationChanged(QString);

public slots:

  virtual void setFile(const QString &file) = 0;
  virtual void setRotation(const QString &rotation) = 0;
};

class INSPECTOR_EXPORT OriTxtFormatWidgetImp
  : public OriTxtFormatWidget
{

  Q_OBJECT

public:

  OriTxtFormatWidgetImp(QWidget *parent = nullptr);
  ~OriTxtFormatWidgetImp() override = default;

protected slots:

  void onPushButtonSelectPath();

// IWidgetView interface

  QString file() const override;
  QString rotation() const override;

public slots:

  void setFile(const QString &file) override;
  void setRotation(const QString &rotation) override;

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
  QLabel *mLabelRotation;
  QComboBox *mComboBoxRotation;
  QLabel *mLabelFile;
  QLineEdit *mLineEditFile;
  QPushButton *mPushButtonSelectPath;
};

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_FORMAT_TXT_WIDGET_H
