#!/bin/bash

if [ -z "$CMAKE_BUILD_TYPE" ]; then
	echo "Define \"CMAKE_BUILD_TYPE\"."
	exit 1
fi

export HOME_TAMINO="/home/tamino"
export ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"
export ANT_PATH=$(which ant)
export ANDROID_API_LEVEL="25"
export ANDROID_KEY_PATH="$HOME_TAMINO/Dokumente/Projekte/fairytale/my-release-key.keystore"

BUILD_DIR="./buildandroid$ANDROID_ABI"

if [ ! -d "$BUILD_DIR" ] ; then
	#rm -rf "$BUILD_DIR"
	mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
echo "Build dir: $(pwd)"

# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake -DCMAKE_VERBOSE_MAKEFILE=on -DUSE_COMPRESSED_FILES=1 -DJAVA_HOME="/usr/java/default/" -DQT_ANDROID_SDK_ROOT="$ANDROID_HOME" -DQT_ANDROID_ANT="$ANT_PATH" -DCMAKE_MODULE_PATH="../cmake" -DCMAKE_PREFIX_PATH="$QT_DIR/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK="$HOME_TAMINO/android-ndk-r12b" -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" -DANDROID_ABI="$ANDROID_ABI" -DANDROID_TOOLCHAIN_NAME="$ANDROID_TOOLCHAIN_NAME" -DANDROID_NATIVE_API_LEVEL="$ANDROID_API_LEVEL" -DANDROID_API_LEVEL="$ANDROID_API_LEVEL" -DQT_DIR="$QT_DIR" -DANDROID_PREFIX="$ANDROID_PREFIX" -DANDROID_PREFIX_FFMPEG="$ANDROID_PREFIX_FFMPEG" -DANDROID_TARGET="$ANDROID_TARGET" -DQTAV_LIB_DIR="$QTAV_LIB_DIR" -DUSE_QTAV=ON -DANDROID_KEY_PATH="$ANDROID_KEY_PATH" -DANDROID_KEY_PASSWORD="$ANDROID_KEY_PASSWORD" ../
make VERBOSE=1 -j4

# Manual installation of the packed APK file for the emulator. There is an option for add_qt_android_apk() as well.
echo "Installing the APK file"
#$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-debug.apk
$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-release-signed.apk
echo "Result: $?"