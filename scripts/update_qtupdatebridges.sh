#!/usr/bin/env bash
git clone https://github.com/TheFutureShell/QtUpdateBridges
cd QtUpdateBridges
export QT_UPD_BRIDGES_COMMIT=$(git rev-parse --short HEAD)
cd ..
rm -rf QtUpdateBridges
echo "$QT_UPD_BRIDGES_COMMIT" > .QtUpdateBridges.weakRef
