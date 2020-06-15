#ifndef INSPECTOR_MATCH_VIEWER_INTERFACES_H
#define INSPECTOR_MATCH_VIEWER_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class MatchViewerModel
  : public IModel
{

  Q_OBJECT

public:

  MatchViewerModel(QObject *parent = nullptr) : IModel(parent) {}
  virtual ~MatchViewerModel() = default;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QString> imagePairs(const QString &imageName) const = 0;
  virtual std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(const QString &imgName1, const QString &imgName2) const = 0;
  virtual void deleteMatch(const QString &imgName1, const QString &imgName2, int query_id, int train_id) = 0;
  //virtual void loadPassPoints() = 0;

public slots:

};


class MatchViewerView
  : public IDialogView
{
  Q_OBJECT

public:

  explicit MatchViewerView(QWidget *parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags())
    : IDialogView(parent, f) {}
  virtual ~MatchViewerView() = default;

  /*!
   * \brief Set the left image
   * \param[in] leftImage Left image
   */
  virtual void setLeftImage(const QString &leftImage) = 0;

  /*!
   * \brief Set the right image
   * \param[in] rightImage Right image
   */
  virtual void setRightImage(const QString &rightImage) = 0;

  /*!
   * \brief Set the list of images for image selector left
   * \param[in] leftImageList List of left images
   */
  virtual void setLeftImageList(const std::vector<QString> &leftImageList) = 0;

  /*!
   * \brief Set the list of images for image selector right
   * \param[in] rightImageList List of right images
   */
  virtual void setRightImageList(const std::vector<QString> &rightImageList) = 0;

  virtual void setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> &matches) = 0;

  virtual void setBGColor(const QString &bgColor) = 0;
  virtual void setSelectedMarkerStyle(const QString &color, int width) = 0;
  virtual void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) = 0;
  virtual void setLineStyle(const QString &color, int width) = 0;

signals:

  void leftImageChange(QString);
  void rightImageChange(QString);
  void loadMatches(QString, QString);
//  void deleteMatch(QString, QString, int, int);
};



class MatchViewerPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  MatchViewerPresenter(){}
  virtual ~MatchViewerPresenter() {}

public slots:

  virtual void openFromImages(const QString &imageLeft, const QString &imageRight) = 0;

protected slots:

  virtual void setLeftImage(const QString &image) = 0;
  virtual void setRightImage(const QString &image) = 0;
  virtual void loadMatches(const QString &imageLeft, const QString &imageRight) = 0;
//  virtual void deleteMatch(const QString &imageLeft, const QString &imageRight, int query_id, int train_id) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_INTERFACES_H
