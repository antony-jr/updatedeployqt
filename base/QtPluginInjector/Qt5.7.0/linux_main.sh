#!/usr/bin/env bash
wget -q "http://download.qt.io/archive/qt/5.7/5.7.0/submodules/qtbase-opensource-src-5.7.0.tar.xz"
tar -xvf qtbase-opensource-src-5.7.0.tar.xz
cp --force qxcbmain.cpp qtbase-opensource-src-5.7.0/src/plugins/platforms/xcb/qxcbmain.cpp
echo "include(../../../../../../QtPluginInjector.pri)" >> qtbase-opensource-src-5.7.0/src/plugins/platforms/xcb/xcb-plugin.pro 
cd qtbase-opensource-src-5.7.0
./configure -shared -release -silent -opensource -confirm-license -opengl -nomake examples -qt-xcb -sm -qt-libpng -no-libjpeg -no-icu -qt-zlib -qt-pcre
make -j$(nproc)
make install
mkdir output
sudo cp -p /usr/local/Qt-5.7.0/plugins/platforms/libqxcb.so ./output/
rm -rf qtbase-opensource-src-5.7.0* # cleanup
sudo chmod -R 755 ./output/*
wget https://raw.githubusercontent.com/probonopd/uploadtool/master/upload.sh
export UPLOADTOOL_SUFFIX="Qt$JOB"
bash upload.sh ./output/*
