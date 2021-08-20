#ifndef GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H
#define GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace graphos
{

class GRAPHOS_EXPORT NvmFormatWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  NvmFormatWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~NvmFormatWidget() override = default;

  virtual QString file() const = 0;

signals:

  void fileChanged(QString);

public slots:

  virtual void setFile(const QString &file) = 0;

};

class GRAPHOS_EXPORT NvmFormatWidgetImp
  : public NvmFormatWidget
{

  Q_OBJECT

public:

  NvmFormatWidgetImp(QWidget *parent = nullptr);
  ~NvmFormatWidgetImp() override = default;

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

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H
