#ifndef INSPECTOR_FEATURES_VIEWER_MODEL_H
#define INSPECTOR_FEATURES_VIEWER_MODEL_H

#include <memory>

#include "inspector/ui/featviewer/FeaturesViewerModel.h"

class QSettings;

namespace inspector
{

class Project;

namespace ui
{

class FeaturesViewerModelImp
  : public FeaturesViewerModel
{

 Q_OBJECT

public:

  FeaturesViewerModelImp(Project *project,
                         QObject *parent = nullptr);
  ~FeaturesViewerModelImp() override;

// FeaturesViewerModel interface

public:

  std::vector<QString> images() const override;
  std::vector<QPointF> loadKeypoints(const QString &imageName) override;

  QString viewerBGColor() const override;
  int viewerMarkerType() const override;
  int viewerMarkerSize() const override;
  int viewerMarkerWidth() const override;
  QString viewerMarkerColor() const override;
  int viewerSelectMarkerWidth() const override;
  QString viewerSelectMarkerColor() const override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QSettings *mSettings;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_MODEL_H
