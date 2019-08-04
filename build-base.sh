#!/usr/bin/env bash

printf "Build Base, A Simple bash script to build required base binaries.\n"
printf "Copyright (C) 2019 The Future Shell Laboratory.\n\n"

if [ $# -lt 3 ]
        then
		printf "Usage: $0 [QT VERSION] [CACHE DIRECTORY] [HPACK BIN]\n"
                exit 0
fi


if [ ! -f ./build-base.sh ]; then
    printf "You are attempting to run the $0 from a wrong directory.\n"
    printf "If you wish to run this script, you'll have to have\n"
    printf "the repository root directory as the working directory.\n\n"
    exit 1
fi

QtVersion="$1"
CacheDir="$2"
HpackBin="$3"

QtVersionNoDot=$(echo $QtVersion | tr -s '.' | tr '.' '_')

bash "./install_build_utils.sh"

printf "Building Qt Plugin Injector: Qt $QtVersion\n"
printf "Using Cache Directory: $CacheDir\n"
printf "Using hpack: $HpackBin\n\n"

cd "base/QtPluginInjector/Qt$QtVersion"
eval "bash linux_main.sh"
cd ../../..

eval "cp -f /usr/local/Qt-$QtVersion/plugins/platforms/libqxcb.so /tmp/"
eval "cp -f /usr/local/Qt-$QtVersion/plugins/platforms/libqxcb.so $CacheDir/libqxcb-$QtVersion.so"
eval "$HpackBin /tmp/libqxcb.so -o $CacheDir/libqxcb_$QtVersionNoDot.h -g INCLUDED_LIBQXCB_$QtVersionNoDot -v libqxcb_$QtVersionNoDot"

# Build all update bridges if its the lowest possible qt build
if [ "$QtVersion" == "5.6.0" ]
	then
		# Build AppImageUpdaterBridge
		cd "base/AppImageUpdaterBridge"
		mkdir build
		cd build
		/usr/local/Qt-$QtVersion/bin/qmake "CONFIG+=FULL_BUILD" ..
		make -j$(nproc)
		eval "cp -f ./libAppImageUpdaterBridge.so $CacheDir/libAppImageUpdaterBridge.so"
		eval "$HpackBin ./libAppImageUpdaterBridge.so -o $CacheDir/aiub.h -g INCLUDED_AIUB_BINARY -v aiub_binary"
fi
