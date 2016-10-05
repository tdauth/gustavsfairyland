#!/bin/bash
ANDROID_ABI="$1"
ANDROID_API_LEVEL="$2"
QT_DIR="$3"

export HOME_TAMINO="/home/tamino"

if [ -z "$ANDROID_ABI" ] ; then
	ANDROID_ABI="armeabi-v7a" # x86
fi

if [ -z "$ANDROID_API_LEVEL" ] ; then
	ANDROID_API_LEVEL="22"
fi

if [ -z "$QT_DIR" ] ; then
	QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0/5.7/android_x86/"
fi

# Parameters for build QtAV
ANDROID_PREFIX="$4"
ANDROID_TARGET="$5"

if [ -z "$ANDROID_PREFIX" ] ; then
	ANDROID_PREFIX="sdk-android-armv7" # sdk-android-x86
fi

if [ -z "$ANDROID_TARGET" ] ; then
	ANDROID_TARGET="armv7" # x86
fi

BUILD_DIR="./buildandroid$ANDROID_ABI"

if [ ! -d "$BUILD_DIR" ] ; then
	mkdir "$BUILD_DIR"
fi

# -DCMAKE_PREFIX_PATH="$HOME/Qt5.7.0/5.7/android_x86/lib/cmake"
echo "$HOME"
export PATH="$HOME_TAMINO/Android/Sdk/tools/:$PATH"
export ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$PATH"
export PATH="$ANDROID_HOME/platform-tools:$PATH"
export ANT_PATH=$(which ant)
cd "$BUILD_DIR"
# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake -DUSE_COMPRESSED_FILES=1 -DJAVA_HOME="/usr/java/default/" -DANDROID_SDK="$ANDROID_HOME" -DQT_ANDROID_SDK_ROOT="$ANDROID_HOME" -DQT_ANDROID_ANT="$ANT_PATH" -DCMAKE_MODULE_PATH="../cmake" -DCMAKE_PREFIX_PATH="$QT_DIR/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK="$HOME_TAMINO/android-ndk-r12b" -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI="$ANDROID_ABI" -DANDROID_NATIVE_API_LEVEL="$ANDROID_API_LEVEL" ../ \
# Parameters for QtAV
-DQT_DIR="$QT_DIR" -DANDROID_PREFIX="$ANDROID_PREFIX" -DANDROID_TARGET="$ANDROID_TARGET"

cmake --build .
# Manual installation of the packed apk file for the emulator
echo "Installing the APK file"
$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-debug.apk
#$ANDROID_HOME/platform-tools/adb install -r ./binQtApp-release-signed.apk
echo "Result: $?"
