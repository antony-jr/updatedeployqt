#!/usr/bin/env bash

printf "Build and Deploy, A Simple bash script to build updatedeployqt binary and deploy.\n"
printf "Copyright (C) 2019 The Future Shell Laboratory.\n\n"

if [ $# -lt 1 ]
        then
		printf "Usage: $0 [CACHE DIRECTORY]\n"
                exit 0
fi


if [ ! -f ./build-tools.sh ]; then
    printf "You are attempting to run the $0 from a wrong directory.\n"
    printf "If you wish to run this script, you'll have to have\n"
    printf "the repository root directory as the working directory.\n\n"
    exit 1
fi

CacheDir="$1"
GitCommit=$(git rev-parse --short HEAD)

mkdir build
cd build
cmake -DGIT_COMMIT_STR="$GitCommit" -DCACHE_DIR="$CacheDir" ..
make -j$(nproc)

mv "./updatedeployqt" "../updatedeployqt-continuous-x86_64.bin"
cd ..
rm -rf build

# Now lets build an AppImage just as an option
mkdir build
cd build
cmake -DGIT_COMMIT_STR="$GitCommit" ..
make -j$(nproc)
make install DESTDIR=../appdir
cd ..
mkdir appdir/data
cd appdir/data
cp "$CacheDir/libqxcb-5.6.0.so" .
cp "$CacheDir/libqxcb-5.7.0.so" .
cp "$CacheDir/libqxcb-5.8.0.so" .
cp "$CacheDir/libqxcb-5.9.0.so" .
cp "$CacheDir/libqxcb-5.10.0.so" .
cp "$CacheDir/libAppImageUpdaterBridge.so" .
cd ../..

wget -q http://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
export VERSION="continuous"
./linuxdeploy-x86_64.AppImage --appdir appdir --output appimage


# Deploy to Github releases
wget https://raw.githubusercontent.com/probonopd/uploadtool/master/upload.sh
bash upload.sh "./updatedeployqt-continuous-x86_64.bin" "updatedeployqt-continuous-x86_64.AppImage" "*zsync"
