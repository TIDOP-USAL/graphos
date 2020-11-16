#ifndef INSPECTOR_POINT_CLOUD_CSV_FORMAT_WIDGET_H
#define INSPECTOR_POINT_CLOUD_CSV_FORMAT_WIDGET_H

#include "inspector/widgets/InspectorWidget.h"


class QGroupBox;
class QLabel;
class QRadioButton;

namespace inspector
{

class INSPECTOR_EXPORT PointCloudCSVFormatWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  PointCloudCSVFormatWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~PointCloudCSVFormatWidget() override = default;

  virtual QString delimiter() const = 0;

signals:

  void delimiterChanged(QString);

public slots:

  virtual void setDelimiter(const QString &delimiter) = 0;

};

class INSPECTOR_EXPORT PointCloudCSVFormatWidgetImp
  : public PointCloudCSVFormatWidget
{

  Q_OBJECT

public:

  PointCloudCSVFormatWidgetImp(QWidget *parent = nullptr);
  ~PointCloudCSVFormatWidgetImp() override = default;

protected slots:

  void onDelimiterChanged();

// PointCloudCSVFormatWidget interface

public:

  QString delimiter() const override;

public slots:

  void setDelimiter(const QString &delimiter) override;

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

  QGroupBox *mGroupBoxDelimiter;
  QRadioButton *mRadioButtonTab;
  QRadioButton *mRadioButtonComma;
  QRadioButton *mRadioButtonSpace;
  QRadioButton *mRadioButtonSemicolon;

};

} // namespace inspector


#endif // INSPECTOR_POINT_CLOUD_CSV_FORMAT_WIDGET_H
