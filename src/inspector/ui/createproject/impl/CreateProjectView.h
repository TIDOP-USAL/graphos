#ifndef INSPECTOR_CREATE_PROJECT_VIEW_H
#define INSPECTOR_CREATE_PROJECT_VIEW_H

#include "inspector/ui/createproject/CreateProjectView.h"

class QLineEdit;
class QCheckBox;
class QDialogButtonBox;
class QTextEdit;
class QPushButton;
class QLabel;

namespace inspector
{

namespace ui
{

class CreateProjectViewImp
  : public CreateProjectView
{
  Q_OBJECT

public:

  explicit CreateProjectViewImp(QWidget *parent = nullptr);
  ~CreateProjectViewImp() override = default;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// CreateProjectView interface

public:

  QString projectName() const override;
  QString projectPath() const override;
  void setProjectPath(const QString &path) override;
  QString projectDescription() const override;
  bool createProjectFolder() const override;
  void setExistingProject(bool prjExist) override;

protected slots:

  void onClickButtonSelectPath() override;

protected:

  QLabel *mLabelProjectName;
  QLineEdit *mLineEditProjectName;
  QLabel *mLabelProjectPath;
  QLineEdit *mLineEditProjectPath;
  QLabel *mLabelProjectFile;
  QLineEdit *mLineEditProjectFile;
  QLabel *mLabelDescription;
  QTextEdit *mTextEditDescription;
  QCheckBox *mCheckBoxProjectFolder;
  QPushButton *mPushButtonProjectPath;
  QDialogButtonBox *mButtonBox;
  bool bPrjExist;
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_CREATE_PROJECT_VIEW_H
