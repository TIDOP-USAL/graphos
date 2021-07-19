#ifndef INSPECTOR_FEATURE_EXTRACTOR_VIEW_H
#define INSPECTOR_FEATURE_EXTRACTOR_VIEW_H

#include "inspector/ui/featextract/FeatureExtractorView.h"

class QGridLayout;
class QComboBox;
class QDialogButtonBox;
class QSpinBox;
class QCheckBox;
class QLabel;

namespace inspector
{

namespace ui
{

class FeatureExtractorViewImp
  : public FeatureExtractorView
{

  Q_OBJECT

public:

  FeatureExtractorViewImp(QWidget *parent = nullptr);
  ~FeatureExtractorViewImp() override;

protected slots:

    void onCheckBoxFullImageChange();

// FeatureExtractorView interface

public:

  void addDetectorDescriptor(QWidget *detectorDescriptor) override;
  QString currentDetectorDescriptor() const override;
  int maxImageSize() const override;
  bool fullImageSize() const override;

public slots:

  void setCurrentDetectorDescriptor(const QString &detectorDescriptor) override;
  void setMaxImageSize(int imageSize) override;
  void setFullImageSize(bool fullImageSize) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGridLayout *mGridLayoutDetectorDescriptor;
  QCheckBox *mCheckBoxFullImage;
  QLabel *mLabelMaxImageSize;
  QSpinBox *mSpinBoxMaxImageSize;
  QLabel *mLabelDetectorDescriptor;
  QComboBox *mComboBoxDetectorDescriptor;
  QDialogButtonBox *mButtonBox;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_VIEW_H
