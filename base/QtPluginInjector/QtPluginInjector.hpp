#ifndef QT_PLUGIN_INJECTOR_HPP_INCLUDED
#define QT_PLUGIN_INJECTOR_HPP_INCLUDED
#include <Timer.hpp>

class QtPluginInjector : public QObject {
	Q_OBJECT
public:
	QtPluginInjector(QObject *parent = nullptr);

public Q_SLOTS:
	void init();

private Q_SLOTS:
	void tryLoadPlugin();
private:
	Timer m_Timer;
};

#endif 
