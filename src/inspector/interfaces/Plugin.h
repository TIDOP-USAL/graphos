#ifndef INSPECTOR_PLUGIN_INTERFACES_H
#define INSPECTOR_PLUGIN_INTERFACES_H


namespace inspector
{

class GraphosPlugin
{

public:

	GraphosPlugin() = default;
	virtual ~GraphosPlugin() = default;


};
Q_DECLARE_INTERFACE(FilterInterface, "tidop.graphos.inspector.GraphosPlugin/2.0")

class GraphosPluginComponent
	: public GraphosPlugin
{

public:
	
	GraphosPluginComponent() = default;
	~GraphosPluginComponent() override = default;

	virtual Component *component() = 0;
};
Q_DECLARE_INTERFACE(FilterInterface, "tidop.graphos.inspector.GraphosPluginComponent/2.0")

} // namespace inspector

#endif // INSPECTOR_PLUGIN_INTERFACES_H
