# updatedeployqt

updatedeployqt is a command line tool which can deploy auto update to any Qt based Application regardless of 
the programming language using the Qt plugin mechanism. This tool was specifically built to provide 
auto update feature for Qt Applications packaged with AppImages , Qt Installer Framework and Qt Applications
simply released through Github releases.

These mode of update is refered as so called bridges , for now only AppImageUpdater bridge is stable and usable.

**IMPORTANT**: This software is in alpha stage. Remember with great power comes great responsibility.

# Installation

You don't need to compile this from source , simply download the latest AppImage from releases and mark it
executable and you are ready to go. This works in travis-ci and other such services too. So you can deploy
update directly on travis-ci.

```
 $ wget "https://github.com/TheFutureShell/updatedeployqt/releases/download/continuous/updatedeployqt-continuous-x86_64.AppImage"
 $ chmod +x updatedeployqt-continuous-x86_64.AppImage
 $ ./updatedeployqt-continuous-x86_64.AppImage
```


# Usage

For now this only covers the usage for deploying auto updater for AppImages.

``` 
   updatedeployqt version 1-alpha (commit none) , built on Tue May 21 15:42:15 2019
   copyright (C) 2019 the future shell laboratory , antony jr.

   Usage: ./updatedeployqt [BRIDGE] [PATH to libqxcb.so] [OPTIONS]

   BRIDGES: 
      AppImage         deploy auto updater for AppImages
      QtInstaller      deploy auto updater for Qt Installer packaged Application
      GithubReleases   deploy auto updater for Qt Application released via Github

   OPTIONS: 
      -q,--qmake       path to qmake binary to use to query qt version.
     -qv,--qt-version  assume this as the qt version.
      -l,--lib-path    path where libraries are deployed.
```


### AppImages

First step is to bundle the required shared libraries and qt plugins using [linuxdeployqt]() or [linuxdeploy]() in the **AppDir**.
You **SHOULD** not package it as AppImage yet. 

Now execute this command with respect to your **AppDir**. You have to find the qxcb plugin typically located in your **AppDir** at
**'AppDir/usr/plugins/platforms/libqxcb.so'**

```
    $ ./updatedeployqt-continuous-x86_64.AppImage AppImage AppDir/usr/plugins/platforms/libqxcb.so
```

**Thats it**.

Now **package it as an AppImage**. You can use [linuxdeployqt]() , [linuxdeploy]() or [appimagetool](). Make sure you have 
included the **update information** as given in the **AppImage specification** , Without the update information embeded in 
the AppImage , the update will not work.

**IMPORTANT** : Also you have to make sure that you deploy **libQt5Network.so.5** which should be deployed automatically by
[linuxdeploy]() in most cases. You can use ```--lib-path``` command argument to check for the network module.

 
**NOTE** : The Qt version number is very important to deploy the correct plugins , the program automatically detects the qt version 
by querying **qmake** installed in the system. You can however can specify any Qt version using ```--qt-version``` program
argument.


#### Repacking AppImages with Auto Update

With this tool you can also repack AppImages with auto update after packaging it , down below is an example on how to
do it. **However** you should know the **Qt major and minor version used in the AppImage.** 

We are repacking **qTox** AppImage along with our Auto updater.

```
   $ wget "https://github.com/qTox/qTox/releases/download/v1.16.3/qTox-v1.16.3.x86_64.AppImage"
   $ chmod +x qTox-v1.16.3.x86_64.AppImage
   $ ./qTox-v1.16.3.x86_64.AppImage --appimage-extract # get the contents
   $ # We know qTox v1.16.3 uses Qt version 5.7.3 or so.
   $ # Download updatedeployqt 
   $ wget "https://github.com/TheFutureShell/updatedeployqt/releases/download/continuous/updatedeployqt-continuous-x86_64.AppImage"
   $ chmod +x updatedeployqt-continuous-x86_64.AppImage
   $ ./updatedeployqt-continuous-x86_64.AppImage AppImage squash-root/local/plugins/platforms/libqxcb.so -qv "5.7.0"
   $ wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
   $ chmod +x appimagetool-x86_64.AppImage
   $ # qTox does not provide any update info , so for demo we created a kind of proxy for that.
   $ ./appimagetool-x86 -u "" \
                        --no-appstream squash-root
   $ # Now you should have qTox-x86_64.AppImage ? Something like that.
   $ ./qTox-x86_64.AppImage # The update should start in a second. 
```


# License

BSD 3-Clause License

Copyright (c) 2019, Future Shell Laboratory.
All rights reserved.




