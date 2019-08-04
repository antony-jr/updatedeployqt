#include <QCoreApplication>
#include <QLibraryInfo>
#include <QCryptographicHash>
#include <QDir>
#ifndef NO_DEBUG
#include <QDebug>
#endif
#include <QMetaObject>
#include <QMetaMethod>
#include <QPluginLoader>
#include <QtPluginInjector.hpp>
#include <AppImageUpdaterBridgeInterface.hpp>


/*
 * Settings for the Plugin Injector.
 * This can be directly overwritten when this is in machine code.
*/
const char PluginToLoadMD5Sum[33] = "8cfaddf5b1a24d1fd31cab97b01f1f87";
const char Bridge[33] = "f80b03178d4080a30c14e71bbbe6e31b";

/*
 * Bridge[0] == 1 then its AppImageUpdater Bridge.
 * Bridge[1] == 1 then its Qt Installer Framework Bridge.
 * Bridge[2] - Bridge[33] yet to be filled.
*/

QtPluginInjector::QtPluginInjector(QObject *parent)
	: QObject(parent)
{
	m_Timer.setInterval(4000);
	connect(&m_Timer , &Timer::timeout , this , &QtPluginInjector::tryLoadPlugin);
}

void QtPluginInjector::init()
{
	m_Timer.start();
	return;
}

void QtPluginInjector::tryLoadPlugin(){
	m_Timer.stop();
	auto instance = QCoreApplication::instance();
	if(!instance){
#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: INFO: no QApplication instance found, retrying.";
#endif
		m_Timer.start();
		return;
	}	

#ifndef NO_DEBUG
	qDebug() << "QtPluginInjector:: INFO: trying to load plugin.";
#endif
	setParent(instance);
	
	/* Search for the plugins in the given plugins directory by
	 * qt. */

	QCryptographicHash *hasher = new QCryptographicHash(QCryptographicHash::Md5);
	QFile *file = new QFile;
	QObject *plugin = nullptr;
	QByteArray requiredHash(PluginToLoadMD5Sum);
	QDir dir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
	auto list = dir.entryList(QDir::Files);
	auto end = list.size();	

#ifndef NO_DEBUG
	qDebug() << "QtPluginInjector:: INFO: searching for required plugins in " 
		 << QLibraryInfo::location(QLibraryInfo::PluginsPath);
#endif
	for(int i = 0; i < end; ++i){ 
		/* Verify MD5 Sum and then load it. */
		hasher->reset();

#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: INFO: checking plugin "
			 << dir.absoluteFilePath(list.at(i));
#endif

		file->setFileName(dir.absoluteFilePath(list.at(i)));
		if(!file->open(QIODevice::ReadOnly)){
#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: WARNING: cannot open file for read.";
#endif
			continue;
		}
		hasher->addData(file);
#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: INFO: comparing hashed(" << hasher->result().toHex() << ") == required("
			 << requiredHash << ")";
#endif
		if(hasher->result().toHex() == requiredHash){
#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: INFO: mdsum matched.";
#endif
			file->close();
			QPluginLoader pluginLoader(dir.absoluteFilePath(list.at(i)));
			plugin = pluginLoader.instance();
			break;
		}
		file->close();
        }
	if(plugin){
		/* IMPORTANT: 
		 *     Do not worry about deallocating plugin QObject since it should be 
		 *     unloaded when the application closes by QPluginLoader automatically.
		 *     If you deallocate the plugin then it would result in segmentation 
		 *     fault. */
		if(Bridge[0] == 1){ // AppImage Updater Bridge.
			AppImageUpdaterBridgeInterface *interface = qobject_cast<AppImageUpdaterBridgeInterface*>(plugin);
			if(interface){
#ifndef NO_DEBUG
			     qDebug() << "QtPluginInjector:: Calling initAppImageUpdaterBridge()";
#endif
			     interface->initAppImageUpdaterBridge();
			}
		}else if(Bridge[1] == 1){ // QInstaller Bridge.
#ifndef NO_DEBUG
			     qDebug() << "QtPluginInjector:: QInstallerBridge is not implemented yet.";
#endif
		}
	}else{
#ifndef NO_DEBUG
		qDebug() << "QtPluginInjector:: FATAL: cannot find the reqired plugin.";
#endif
	}
	delete hasher;
	delete file;
}
