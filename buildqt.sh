#!/bin/bash

# This script builds Qt5 for Android. It requires Java, the Android NDK and the Android SDK.

if [ -z "$ANDROID_NDK_HOST" ] ; then
	echo "Specify \"ANDROID_NDK_HOST\"!"
	exit 1
fi

if [ -z "$ANDROID_TOOLCHAIN_VERSION" ] ; then
	echo "Specify \"ANDROID_TOOLCHAIN_VERSION\"!"
	exit 1
fi

if [ ! -d ./qt5 ] ; then
	git clone git://code.qt.io/qt/qt5.git qt5
fi

cd ./qt5
# After init-repository!
git checkout 5.7.1
git submodule update --init
HOME_TAMINO="/home/tamino"
JAVA_HOME=/usr/java/default/ # You need a JDK (version 1.6+, required 32bit) and you should also have this in your PATH.
ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
PATH="$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"
ANDROID_SDK_ROOT="$ANDROID_HOME"
ANDROID_NDK_ROOT="$HOME_TAMINO/android-ndk-r12b"
# Don't use -developer-build, we want a normal release installation.
./configure -prefix "$WORKSPACE/install" -confirm-license -opensource -xplatform android-g++ -nomake tests -nomake examples -android-ndk "$ANDROID_NDK_ROOT" -android-sdk "$ANDROID_SDK_ROOT" -android-ndk-host "$ANDROID_NDK_HOST" -android-toolchain-version "$ANDROID_TOOLCHAIN_VERSION" -skip qttranslations -skip qtwebkit -skip qtserialport -skip qtwebkit-examples -no-warnings-are-errors
make -j4 install # is not needed when -prefix is used