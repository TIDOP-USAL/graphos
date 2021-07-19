#ifndef INSPECTOR_FEATURES_MODEL_H
#define INSPECTOR_FEATURES_MODEL_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class FeaturesModel
  : public Model
{

  Q_OBJECT

public:

  typedef std::map<QString, QString>::iterator features_iterator;
  typedef std::map<QString, QString>::const_iterator features_const_iterator;


public:

  FeaturesModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~FeaturesModel() override = default;

  virtual QString features(const QString &imgName) const = 0;
  virtual void addFeatures(const QString &imgName, const QString &featureFile) = 0;
  virtual bool removeFeatures(const QString &imgName) const = 0;

  virtual features_iterator begin() = 0;
  virtual features_const_iterator begin() const = 0;
  virtual features_iterator end() = 0;
  virtual features_const_iterator end() const = 0;

};


class FeaturesModelImp
  : public FeaturesModel
{

  Q_OBJECT

public:

  FeaturesModelImp(Project *project,
                   QObject *parent = nullptr);
  ~FeaturesModelImp() override = default;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

// FeaturesModel interface

public:

  QString features(const QString &imgName) const override;
  void addFeatures(const QString &imgName, const QString &featureFile) override;
  bool removeFeatures(const QString &imgName) const override;

  features_iterator begin() override;
  features_const_iterator begin() const override;
  features_iterator end() override;
  features_const_iterator end() const override;

protected:

  Project *mProject;
};


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_MODEL_H
