/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/


#ifndef GRAPHOS_MVP_H
#define GRAPHOS_MVP_H

#include <memory>

#include <QObject>
#include <QDialog>
#include <QEvent>
#include <QDockWidget>


namespace tl
{
class Command;
}

namespace graphos
{

class HelpDialog;

using View = QObject;

class CommandView
  : public View
{

public:

  CommandView(QObject *parent = nullptr)
    : View(parent) { }
  virtual ~CommandView() override = default;

  virtual std::shared_ptr<tl::Command> command() = 0;

};


class DialogView
  : public QDialog
{
  Q_OBJECT

public:

  DialogView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
    : QDialog(parent, f)
  {
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~helpFlag);
    QWidget::setWindowFlags(flags);
  }

  virtual ~DialogView() override = default;

private:

  /*!
   * \brief Dialog initialization UI
   */
  virtual void initUI() = 0;

  /*!
   * \brief Signal and slots initialization
   */
  virtual void initSignalAndSlots() = 0;

signals:

  void help();
  void restore();

public slots:

  /*!
   * \brief Clear dialog
   */
  virtual void clear() = 0;

private slots:

  virtual void update() = 0;
  virtual void retranslate() = 0;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override
  {
    if (event->type() == QEvent::LanguageChange){
      retranslate();
    }
  }
};



class DockWidgetView
  : public QDockWidget
{
  Q_OBJECT

public:

  DockWidgetView(QWidget *parent = nullptr)
    : QDockWidget(parent)
  {
  }

  virtual ~DockWidgetView() override = default;

private:

  virtual void initUI() = 0;
  virtual void initSignalAndSlots() = 0;

signals:

  //void help();
  void restore();

public slots:

  virtual void clear() = 0;

private slots:

  virtual void update() = 0;
  virtual void retranslate() = 0;

  // QWidget interface

protected:

  void changeEvent(QEvent *event) override
  {
    if(event->type() == QEvent::LanguageChange) {
      retranslate();
    }
  }
};



class Model
  : public QObject
{

  Q_OBJECT

public:

  Model(QObject *parent = nullptr) : QObject(parent){}
  virtual ~Model() = default;

private:

  /*!
   * \brief Class Initialization
   */
  virtual void init() = 0;

public slots:

  virtual void clear() = 0;

};


class Presenter
  : public QObject
{
  Q_OBJECT

public:

  Presenter() : QObject(){}
  virtual ~Presenter() = default;

public slots:

 /*!
  * \brief Show help
  */
  //virtual void help() = 0;

 /*!
  * \brief Open
  */
  virtual void open() = 0;

  /*!
   * \brief Set Help
   * \param[in] help
   */
  //virtual void setHelp(HelpDialog *help) = 0;

private:

  /*!
   * \brief Class Initialization
   */
  virtual void init() = 0;

  /*!
   * \brief Signal and slots initialization
   */
  virtual void initSignalAndSlots() = 0;


signals:

  void help(QString);

};


} // namespace graphos

#endif // GRAPHOS_MVP_H
