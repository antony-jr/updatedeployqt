#!/usr/bin/env bash

if [ "$JOB" == "5.6.0" ]
	then
		cd Qt5.6.0
		sudo bash linux_main.sh # Build modified libqxcb.so plugin for Qt5.6
		cd ..
fi

if [ "$JOB" == "5.7.0" ]
	then
		cd Qt5.7.0
		sudo bash linux_main.sh # Build modified libqxcb.so plugin for Qt5.7
		cd ..
fi

if [ "$JOB" == "5.8.0" ]
	then
		cd Qt5.8.0
		sudo bash linux_main.sh # Build modified libqxcb.so plugin for Qt5.8
		cd ..
fi


if [ "$JOB" == "5.9.0" ]
	then
		cd Qt5.9.0
		sudo bash linux_main.sh # Build modified libqtxcb.so plugin for Qt5.9
		cd ..
fi

if [ "$JOB" == "5.10.0" ]
	then
	       cd Qt5.10.0
	       sudo bash linux_main.sh 
	       cd ..
fi
