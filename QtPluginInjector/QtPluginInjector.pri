INCLUDEPATH += $$PWD
QT += core
HEADERS += $$PWD/Timer_p.hpp \
           $$PWD/Timer.hpp \
           $$PWD/QtPluginInjector.hpp

SOURCES += $$PWD/Timer_p.cc \
           $$PWD/Timer.cc \
           $$PWD/QtPluginInjector.cc

NO_DEBUG {
	message(debug messages will be not be compiled in this build.)
	DEFINES += NO_DEBUG
}
