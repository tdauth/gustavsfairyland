#!/bin/bash
ANDROID_ABI="$1"
ANDROID_TOOLCHAIN_NAME="$2"
ANDROID_API_LEVEL="$3"
QT_DIR="$4"

export HOME_TAMINO="/home/tamino"

if [ -z "$ANDROID_ABI" ] ; then
	echo "Setting ANDROID_ABI automatically"
	ANDROID_ABI="armeabi-v7a" # x86
fi

if [ -z "$ANDROID_TOOLCHAIN_NAME" ] ; then
	echo "Setting ANDROID_TOOLCHAIN_NAME automatically"
	ANDROID_TOOLCHAIN_NAME="arm-linux-androideabi-4.9" # "x86-4.9"
fi

if [ -z "$ANDROID_API_LEVEL" ] ; then
	echo "Setting ANDROID_API_LEVEL automatically"
	ANDROID_API_LEVEL="22"
fi

if [ -z "$QT_DIR" ] ; then
	echo "Setting QT_DIR automatically"
	QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0/5.7/android_x86/"
fi

# Parameters for build QtAV
ANDROID_PREFIX="$5"
ANDROID_TARGET="$6"
QTAV_LIB_DIR="$7"

if [ -z "$ANDROID_PREFIX" ] ; then
	echo "Setting ANDROID_PREFIX automatically"
	ANDROID_PREFIX="sdk-android-armv7" # sdk-android-x86
fi

if [ -z "$ANDROID_TARGET" ] ; then
	echo "Setting ANDROID_TARGET automatically"
	ANDROID_TARGET="armv7" # x86
fi

if [ -z "$QTAV_LIB_DIR" ] ; then
	echo "Setting QTAV_LIB_DIR automatically"
	QTAV_LIB_DIR="lib_android_arm" # lib_android_x86
fi

export ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"
export ANT_PATH=$(which ant)

BUILD_DIR="./buildandroid$ANDROID_ABI"

if [ ! -d "$BUILD_DIR" ] ; then
	mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
echo "Build dir: $(pwd)"
# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake -DUSE_COMPRESSED_FILES=1 -DJAVA_HOME="/usr/java/default/" -DANDROID_SDK="$ANDROID_HOME" -DQT_ANDROID_SDK_ROOT="$ANDROID_HOME" -DQT_ANDROID_ANT="$ANT_PATH" -DCMAKE_MODULE_PATH="../cmake" -DCMAKE_PREFIX_PATH="$QT_DIR/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK="$HOME_TAMINO/android-ndk-r12b" -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI="$ANDROID_ABI" -DANDROID_TOOLCHAIN_NAME="$ANDROID_TOOLCHAIN_NAME" -DANDROID_NATIVE_API_LEVEL="$ANDROID_API_LEVEL" -DQT_DIR="$QT_DIR" -DANDROID_PREFIX="$ANDROID_PREFIX" -DANDROID_TARGET="$ANDROID_TARGET" -DQTAV_LIB_DIR="$QTAV_LIB_DIR" ../

cmake --build .
# Manual installation of the packed apk file for the emulator
echo "Installing the APK file"
$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-debug.apk
#$ANDROID_HOME/platform-tools/adb install -r ./binQtApp-release-signed.apk
echo "Result: $?"
