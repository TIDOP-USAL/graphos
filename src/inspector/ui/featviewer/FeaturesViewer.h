#ifndef INSPECTOR_FEATURES_VIEWER_INTERFACES_H
#define INSPECTOR_FEATURES_VIEWER_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class FeaturesViewerModel
  : public IModel
{

  Q_OBJECT

public:

  FeaturesViewerModel(QObject *parent = nullptr) : IModel(parent) {}
  ~FeaturesViewerModel() = default;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QPointF> loadKeypoints(const QString &image) = 0;

};


class FeaturesViewerView
  : public IDialogView
{
  Q_OBJECT

public:

  explicit FeaturesViewerView(QWidget *parent = nullptr,
                               Qt::WindowFlags f = Qt::WindowFlags())
       : IDialogView(parent, f) {}
  ~FeaturesViewerView() = default;

  /*!
   * \brief Set the list of images
   * \param[in] imageList List of images
   */
  virtual void setImageList(const std::vector<QString> &imageList) = 0;

  /*!
   * \brief setCurrentImage
   * \param leftImage
   */
  virtual void setCurrentImage(const QString &leftImage) = 0;

  /*!
   * \brief setPoints
   * \param[in] keyPoints Keypoints
   */
  virtual void setKeyPoints(const std::vector<QPointF> &keyPoints) = 0;

  virtual void setBGColor(const QString &bgColor) = 0;
  virtual void setSelectedMarkerStyle(const QString &color, int width) = 0;
  virtual void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) = 0;

signals:

  void imageChange(QString);

};



class FeaturesViewerPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  FeaturesViewerPresenter() {}
  ~FeaturesViewerPresenter() {}

public slots:

  virtual void setImageActive(const QString &image) = 0;
  virtual void openKeypointsFromImage(const QString &image) = 0;

protected slots:

  virtual void loadKeypoints(const QString &image) = 0;

};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_INTERFACES_H
