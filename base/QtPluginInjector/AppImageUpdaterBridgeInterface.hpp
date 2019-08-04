#ifndef APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED 
#define APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED 
#include <QtPlugin>

class AppImageUpdaterBridgeInterface {
public:
	virtual ~AppImageUpdaterBridgeInterface() = 0;
public Q_SLOTS:
	virtual void initAppImageUpdaterBridge() = 0;
};

#ifndef AppImageUpdaterBridgeInterface_iid
#define AppImageUpdaterBridgeInterface_iid "com.antony-jr.AppImageUpdaterBridge"
#endif 

Q_DECLARE_INTERFACE(AppImageUpdaterBridgeInterface , AppImageUpdaterBridgeInterface_iid);

#endif // APPIMAGE_UPDATER_BRIDGE_INTERFACE_HPP_INCLUDED
