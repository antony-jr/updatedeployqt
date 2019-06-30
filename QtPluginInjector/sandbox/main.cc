#include <QApplication>
#include <QtPluginInjector.hpp>

int main(int ac , char **av){
	QApplication app(ac,av);
	QtPluginInjector Injector;
	Injector.init();
	return app.exec();
}
