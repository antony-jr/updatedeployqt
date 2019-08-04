#!/usr/bin/env bash
wget -q "http://download.qt.io/archive/qt/5.6/5.6.0/submodules/qtbase-opensource-src-5.6.0.tar.xz"
tar -xvf qtbase-opensource-src-5.6.0.tar.xz
cp --force qxcbmain.cpp qtbase-opensource-src-5.6.0/src/plugins/platforms/xcb/qxcbmain.cpp
echo "include(../../../../../../QtPluginInjector.pri)" >> qtbase-opensource-src-5.6.0/src/plugins/platforms/xcb/xcb-plugin.pro 
cd qtbase-opensource-src-5.6.0
./configure -shared -release -silent -opensource -confirm-license -opengl -nomake examples -qt-xcb -sm -qt-libpng -no-libjpeg -no-icu -qt-zlib -qt-pcre
make -j$(nproc)
make install
rm -rf qtbase-opensource-src-5.6.0* # cleanup
