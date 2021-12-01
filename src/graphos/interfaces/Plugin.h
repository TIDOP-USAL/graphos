/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_PLUGIN_INTERFACES_H
#define GRAPHOS_PLUGIN_INTERFACES_H

#include <memory>

#include <QtPlugin>
#include <QString>

namespace graphos
{

class Application;
class Component;

class GraphosPlugin
{

public:
  
  GraphosPlugin() = default;
  virtual ~GraphosPlugin() = default;

  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual void setApp(Application *application) = 0;
};


class GraphosPluginComponent
  : public GraphosPlugin
{

public:
	
  GraphosPluginComponent() = default;
  ~GraphosPluginComponent() override = default;

  virtual Component *component() = 0;
};



class GraphosPluginMultiComponent
	: public GraphosPlugin
{

public:
	
  GraphosPluginMultiComponent() = default;
  ~GraphosPluginMultiComponent() override = default;

  virtual QString menu() const = 0;
  virtual QString toolbar() const = 0;
  virtual std::list<std::shared_ptr<graphos::Component>> components() = 0;
  //virtual graphos::Component *component(size_t id) = 0;
  //virtual size_t size() const = 0;
};


} // namespace graphos

Q_DECLARE_INTERFACE(graphos::GraphosPlugin, "tidop.graphos.inspector.plugin.interface/2.0")
Q_DECLARE_INTERFACE(graphos::GraphosPluginComponent, "tidop.graphos.inspector.plugin.component/2.0")
Q_DECLARE_INTERFACE(graphos::GraphosPluginMultiComponent, "tidop.graphos.inspector.plugin.multicomponent/2.0")

#endif // GRAPHOS_PLUGIN_INTERFACES_H
