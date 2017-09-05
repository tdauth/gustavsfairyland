#!/bin/bash
# This scripts helps to compile the project for Android.
# It requires a number of specified environment variables which specify the Android target platform for CMake and Qt.

if [ -z "$CMAKE_BUILD_TYPE" ]; then
	echo "Define \"CMAKE_BUILD_TYPE\"."
	exit 1
fi

export HOME_TAMINO="/home/tamino"
export ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"
export ANDROID_API_LEVEL="24"
export ANDROID_KEY_PATH="$HOME_TAMINO/Dokumente/Projekte/fairytale/my-release-key.keystore"
export ANDROID_NDK="/mnt/ntfs/android-ndk-r15c"

BUILD_DIR="./buildandroid$ANDROID_ABI"

if [ ! -d "$BUILD_DIR" ] ; then
	#rm -rf "$BUILD_DIR"
	mkdir "$BUILD_DIR"
fi

cd "$BUILD_DIR"
echo "Build dir: $(pwd)"

# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake \
-DCMAKE_SYSTEM_NAME=Android \
-DCMAKE_SYSTEM_VERSION="$ANDROID_API_LEVEL" \
-DCMAKE_ANDROID_ARCH_ABI="$ANDROID_ABI" \
-DCMAKE_ANDROID_NDK="$ANDROID_NDK" \
-DCMAKE_ANDROID_STL_TYPE=gnustl_static \
-DCMAKE_VERBOSE_MAKEFILE=on \
-DUSE_COMPRESSED_FILES=1 \
-DJAVA_HOME="/usr/java/default/" \
-DQT_ANDROID_SDK_ROOT="$ANDROID_HOME" \
-DCMAKE_MODULE_PATH="../cmake" \
-DCMAKE_PREFIX_PATH="$QT_DIR/lib/cmake" \
-DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
-DQT_DIR="$QT_DIR" \
-DQT_ANDROID_NDK_ROOT="$ANDROID_NDK" \
-DQT_ANDROID_SDK_BUILDTOOLS_REVISION="22" \
-DANDROID_TOOLCHAIN_MACHINE_NAME="$ANDROID_TOOLCHAIN_NAME" \
-DANDROID_COMPILER_VERSION="$ANDROID_COMPILER_VERSION" \
-DANDROID_ABI="$ANDROID_ABI" \
-DANDROID_NDK_HOST_SYSTEM_NAME="$ANDROID_NDK_HOST_SYSTEM_NAME" \
-DQT_ANDROID_SDK_BUILDTOOLS_REVISION="23.0.3" \
-DANDROID_NATIVE_API_LEVEL="$ANDROID_API_LEVEL" \
-DANDROID_PREFIX="$ANDROID_PREFIX" \
-DANDROID_PREFIX_FFMPEG="$ANDROID_PREFIX_FFMPEG" \
-DANDROID_TARGET="$ANDROID_TARGET" \
-DQTAV_LIB_DIR="$QTAV_LIB_DIR" \
-DUSE_QTAV=ON \
-DANDROID_KEY_PATH="$ANDROID_KEY_PATH" \
-DANDROID_KEY_PASSWORD="$ANDROID_KEY_PASSWORD" \
../
make VERBOSE=1 -j4

# Manual installation of the packed APK file for the emulator. There is an option for add_qt_android_apk() as well.
echo "Installing the APK file"
#$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-debug.apk
$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-release-signed.apk
echo "Result: $?"