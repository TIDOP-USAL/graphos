#ifndef GRAPHOS_HELP_DIALOG_H
#define GRAPHOS_HELP_DIALOG_H

#include "graphos/interfaces/mvp.h"

#include <QTextBrowser>

class QHelpEngine;

class HelpBrowser
  : public QTextBrowser
{

public:

  HelpBrowser(QHelpEngine *helpEngine,
              QWidget *parent = nullptr);

  QVariant loadResource (int type, const QUrl &name);


private:

  QHelpEngine *mHelpEngine;
};




namespace graphos
{

namespace ui
{
	
class HelpDialog
  : public QDialog
{

  Q_OBJECT

public:

  HelpDialog(QWidget *parent = nullptr);
  ~HelpDialog() override;

public slots:

 /*!
  * \brief Pagina inicial
  */
 void navigateHome();

  /*!
   * \brief Establece la página de ayuda
   * \param[in] page Página de ayuda
   */
  void setPage(const QString &page);

private slots:


  void navigateBackward();
  void navigateForward();
  void setContentSource(const QModelIndex &index);
  void setIndexSource(const QModelIndex &index);
  void searchInvoked();
  void searchFinished(int hits);

// DialogView interface

private:

  void init();

private slots:

  void retranslate();

private:

  QHelpEngine *mHelpEngine;
  HelpBrowser *mHelpBrowser;
  QTextBrowser *mSearchResultsWidget;
  QAction *mNavigateForwardAction;
};

} // namespace ui

} // namespace graphos


#endif // GRAPHOS_HELP_DIALOG_H
