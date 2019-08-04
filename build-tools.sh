#!/usr/bin/env bash

printf "Build Tools, A Simple bash script to build required build tool binaries.\n"
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

cd tools
cd hpack
mkdir build
cmake ..
make -j$(nproc)
eval "cp -p hpack $CacheDir/hpack"
