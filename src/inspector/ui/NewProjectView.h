#ifndef INSPECTOR_NEW_PROJECT_VIEW_H
#define INSPECTOR_NEW_PROJECT_VIEW_H

#include "inspector/ui/NewProject.h"

class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QTextEdit;
class QPushButton;

namespace inspector
{

namespace ui
{

class NewProjectViewImp
  : public NewProjectView
{
  Q_OBJECT

public:

  explicit NewProjectViewImp(QWidget *parent = nullptr);
  ~NewProjectViewImp() override;

// INewProjectView interface

public:

  QString projectName() const override;
  QString projectPath() const override;
  void setProjectPath(const QString &path) override;
  QString projectDescription() const override;
  bool createProjectFolder() const override;

protected slots:

  void onClickButtonSelectPath() override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLineEdit *mLineEditProjectName;
  QLineEdit *mLineEditProjectPath;
  QLineEdit *mLineEditProjectFile;
  QTextEdit *mTextEditDescription;
  QCheckBox *mCheckBoxProjectFolder;
  QPushButton *mPushButtonProjectPath;
  QDialogButtonBox *mButtonBox;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_NEW_PROJECT_VIEW_H
