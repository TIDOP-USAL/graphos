#ifndef GRAPHOS_CMVS_PMVS_WIDGET_H
#define GRAPHOS_CMVS_PMVS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

/*!
 * \brief Shading-Aware Multi-View Stereo Widget Interface
 */
class GRAPHOS_EXPORT CmvsPmvsWidget
  : public IWidgetView
{
  Q_OBJECT

public:

  CmvsPmvsWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  virtual ~CmvsPmvsWidget() = default;

  virtual bool useVisibilityInformation() const = 0;
  virtual int imagesPerCluster() const  = 0;
  virtual int level() const = 0;
  virtual int cellSize() const = 0;
  virtual double threshold() const = 0;
  virtual int windowSize() const = 0;
  virtual int minimunImageNumber() const = 0;
  
signals:

  void useVisibilityInformationChanged(bool);
  void imagesPerClusterChanged(int);
  void levelChanged(int);
  void cellSizeChanged(int);
  void thresholdChanged(double);
  void windowSizeChanged(int);
  void minimunImageNumberChanged(int);
  
public slots:

  virtual void setUseVisibilityInformation(bool useVisibilityInformation) = 0;
  virtual void setImagesPerCluster(int imagesPerCluster)  = 0;
  virtual void setLevel(int level) = 0;
  virtual void setCellSize(int cellSize) = 0;
  virtual void setThreshold(double threshold) = 0;
  virtual void setWindowSize(int windowSize) = 0;
  virtual void setMinimunImageNumber(int minimunImageNumber) = 0;
};


class GRAPHOS_EXPORT CmvsPmvsWidgetImp
  : public CmvsPmvsWidget
{
  Q_OBJECT

public:

  CmvsPmvsWidgetImp(QWidget *parent = nullptr);
  ~CmvsPmvsWidgetImp() override;

// ISmvsWidget interface

public:

  bool useVisibilityInformation() const override;
  int imagesPerCluster() const  override;
  int level() const override;
  int cellSize() const override;
  double threshold() const override;
  int windowSize() const override;
  int minimunImageNumber() const override;
  
public slots:

  void setUseVisibilityInformation(bool useVisibilityInformation) override;
  void setImagesPerCluster(int imagesPerCluster) override;
  void setLevel(int level) override;
  void setCellSize(int cellSize) override;
  void setThreshold(double threshold) override;
  void setWindowSize(int windowSize) override;
  void setMinimunImageNumber(int minimunImageNumber) override;
  
// IWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

protected:
  
  QGroupBox *mGroupBox;
  QCheckBox *mCheckBoxUseVisibilityInformation;
  QLabel *mLabelImagesPerCluster;
  QSpinBox *mSpinBoxImagesPerCluster;
  QLabel *mLabelLevel;
  QSpinBox *mSpinBoxLevel;
  QLabel *mLabelCellSize;
  QSpinBox *mSpinBoxCellSize;
  QLabel *mLabelThreshold;
  QDoubleSpinBox *mDoubleSpinBoxThreshold;
  QLabel *mLabelWindowSize;
  QSpinBox *mSpinBoxWindowSize;
  QLabel *mLabelMinimunImageNumber;
  QSpinBox *mSpinBoxMinimunImageNumber;

};

} // namespace graphos

#endif // GRAPHOS_CMVS_PMVS_WIDGET_H
