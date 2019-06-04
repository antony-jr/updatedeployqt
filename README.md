# Update Deploy Qt ![issues](https://img.shields.io/github/issues/TheFutureShell/updatedeployqt.svg?style=flat-square) ![forks](https://img.shields.io/github/forks/TheFutureShell/updatedeployqt.svg?style=flat-square) ![stars](https://img.shields.io/github/stars/TheFutureShell/updatedeployqt.svg?style=flat-square) ![license](https://img.shields.io/github/license/TheFutureShell/updatedeployqt.svg?style=flat-square)



<p align=center>
<img src="https://raw.githubusercontent.com/TheFutureShell/artwork/master/updatedeployqt/release.png" width="300px" height=auto />
</p>


**updatedeployqt** is a command line tool which can **deploy** auto update feature or *integrate the updater into
your* QApplication with **zero code change.** This does **not depend** on the programming language you used to 
build the **Application** , *as long as it uses Qt framework , this feature can be deployed using this tool.*



**IMPORTANT**: *This software is in alpha stage.* **Remember with great power comes great responsibility.**



# Features

* *Deploy Native Auto Updater with* **zero code change** - *This done using Qt Plugin Injector*.

* **Programming Language Independent** - The reason you want to use this solution.

* *Supports AppImages , Qt Installer Framework and Applications released through Github.*

* *Light-weight* - Using this only takes about **800 KiB**.

* *Cross-platform* - Yet to implement but its going to be.

* *Respects General Data Protection Regulation* - unlike electron-updater.

* *Drink the Qt Kool-aid* - In a positive way.


# Installation

### Linux 

You don't need to compile this from source , simply download the latest AppImage from releases and make it
executable and you are ready to go. This works in travis-ci and other such services too. So you can deploy
update directly on continuous integration services.

You can download the latest AppImage from this [url](https://github.com/TheFutureShell/updatedeployqt/releases/download/continuous/updatedeployqt-continuous-x86_64.AppImage). This *url* will be constant and will never change and thus
you can use this *url* to download the AppImage using wget whenever you want to use it.

```
 $ wget -O updatedeployqt-x86_64.AppImage "https://git.io/fj4CH"
 $ chmod +x updatedeployqt-x86_64.AppImage
 $ ./updatedeployqt-x86_64.AppImage
```

### Windows

Not yet implemented

### MacOSX

Not yet implemented


# Usage

The program looks for **updatedeployqt.json** in the current working directory or in the path 
given through ```-c,--config``` argument. This configuration file can be interactively generated
by passing ```-g,--generate-config``` argument to the program.

This configuration file will decide on how the updater should work , such as auto update without
the users concern or integrate the updater into a menu bar item.

After generating the configuration file , you can simply run the below command to deploy your 
updater.


```
 $ updatedeployqt [PATH TO DEPLOY DIRECTORY]
```

or if **updatedeployqt.json** is not in the current working directory.

```
 $ updatedeployqt -c[PATH TO CONFIG FILE] [PATH TO DEPLOY DIRECTORY]
```


The **deploy directory** is a directory which you have finalized and packaged all required libraries 
and plugins along with your executable and resources. In case you are packaging **AppImages** then 
the **deploy directory** is your **AppDir**. 

# Projects Using This

Feel free to open a **PR** to include your project too. Here are some known projects which use this to deploy
auto update feature for their Qt application.

* [Blue Glow](https://github.com/antony-jr/blue-glow) - You can use this as an example.

* [QTalarm-AppImage](https://github.com/antony-jr/QTalarm-AppImage) - You can also use this as an example.



# License

BSD 3-Clause License

Copyright (c) 2019, Future Shell Laboratory.   
All Rights Reserved.




