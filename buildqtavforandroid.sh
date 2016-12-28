#!/bin/bash

# This script builds the project QtAV for Android which requires ffmpeg as well. Therefore ffmpeg has to be build as well.
# To build both projects for Android, the Android NDK is required as well as a binary Qt version for Android.

BUILD_TYPE="$1"
PROJECT_DIR="$2"
PROJECT_BIN_DIR="$3"
QT_DIR="$4"
ANDROID_PREFIX="$5"
ANDROID_PREFIX_FFMPEG="$6"
ANDROID_TARGET="$7"

export HOME_TAMINO="/home/tamino"

if [ -z "$BUILD_TYPE" ] ; then
	echo "Define BUILD_TYPE as argument 1 (Debug or Release)"
	exit 1
fi

if [ -z "$PROJECT_DIR" ] ; then
	echo "Define PROJECT_DIR as argument 2"
	exit 1
fi

if [ -z "$PROJECT_BIN_DIR" ] ; then
	echo "Define PROJECT_BIN_DIR as argument 3"
	exit 1
fi

if [ -z "$QT_DIR" ] ; then
	echo "Setting QT_DIR automatically"
	QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0"
fi

if [ -z "$ANDROID_PREFIX" ] ; then
	echo "Setting ANDROID_PREFIX automatically"
	ANDROID_PREFIX="sdk-android-armv7-gcc" # sdk-android-x86
fi

if [ -z "$ANDROID_PREFIX_FFMPEG" ] ; then
	echo "Setting ANDROID_PREFIX_FFMPEG automatically"
	ANDROID_PREFIX_FFMPEG="sdk-android-armv7-gcc" # sdk-android-x86-gcc
fi

if [ -z "$ANDROID_TARGET" ] ; then
	echo "Setting ANDROID_TARGET automatically"
	ANDROID_TARGET="armv7" # x86
fi

export NDK_ROOT="$HOME_TAMINO/android-ndk-r12b"
export INSTALL_DIR="../installffmpeg"
export QT_PATH="$QT_DIR"

# Work in the binary dir.
cd "$PROJECT_BIN_DIR"

export FFMPEG_DIR="$PROJECT_BIN_DIR/build_ffmpeg"

# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d "$FFMPEG_DIR" ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git "$FFMPEG_DIR"
fi

export FFMPEG_VERSION="3.1.5"
export FFSRC="$PROJECT_BIN_DIR/ffmpeg-$FFMPEG_VERSION" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
export FFMPEG_PREFIX="$FFMPEG_DIR/$ANDROID_PREFIX_FFMPEG"

# Download and extract ffmpeg if it does not exist.
# TODO checksums? or rather use a local copy
if [ ! -d "$FFSRC" ] ; then
	wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2"
	#wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2.asc"
	#gpg --verify "ffmpeg-$FFMPEG_VERSION.tar.bz2.asc" "ffmpeg-$FFMPEG_VERSION.tar.bz2"
	tar -xjf "./ffmpeg-$FFMPEG_VERSION.tar.bz2"
fi

# Overwrite custom Android configuration with the correct NDK path
cp -f "$PROJECT_DIR/config-android.sh" "$FFMPEG_DIR"
# TODO specify NDK_ROOT in file "config-android.sh" as well as static build
cd "$FFMPEG_DIR"
./build_ffmpeg.sh android "$ANDROID_TARGET"

export QT_AV_DIR="$PROJECT_BIN_DIR/qtav"

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d "$QT_AV_DIR" ]; then
	git clone https://github.com/wang-bin/QtAV.git "$QT_AV_DIR"
fi

cd "$QT_AV_DIR"
git submodule update --init
#git checkout tags/v1.11.0

export QT_AV_BUILD_DIR="$PROJECT_BIN_DIR/buildqtav"

# Always clean the build directory to avoid old stuff.
if [ ! -d "$QT_AV_BUILD_DIR" ]; then
	mkdir "$QT_AV_BUILD_DIR"
fi

cd "$QT_AV_BUILD_DIR"

export FFMPEG_INCLUDE_DIR="$FFMPEG_PREFIX/include"
export FFMPEG_LIB_DIR="$FFMPEG_PREFIX/lib"
export CPATH="$FFMPEG_INCLUDE_DIR/:$CPATH"
export LIBRARY_PATH="$FFMPEG_LIB_DIR:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$FFMPEG_LIB_DIR:$LD_LIBRARY_PATH"
export ANDROID_NDK_ROOT="$NDK_ROOT"

echo "LIBRARY_PATH: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "ls -lha $FFMPEG_LIB_DIR/:"
ls -lha "$FFMPEG_LIB_DIR/"
echo "Building QtAV from: $(pwd)"
echo "Running qmake: \"$QT_PATH/bin/qmake\""
# "$QT_PATH/5.7/android_x86/bin/qmake"
# If the error "Error: libavresample or libswresample is required" appears, use this:
# https://github.com/wang-bin/QtAV/issues/744
# Add the options "CONFIG+=config_avutil config_avformat config_avcodec config_swscale config_swresample" to the user.conf file if "CONFIG += no_config_tests" is used.
# "CONFIG+=debug" is required for the debugging output.
CONFIG_DEBUG=""

if [ "$BUILD_TYPE" = "Debug" ] ; then
	CONFIG_DEBUG="CONFIG+=debug"
else
	CONFIG_DEBUG="CONFIG+=release"
fi

"$QT_PATH/bin/qmake" -Wall "LIBS += -L$FFMPEG_LIB_DIR -lavresample -lswresample" "INCLUDE += -I$FFMPEG_INCLUDE_DIR" "$CONFIG_DEBUG" "CONFIG += config_avutil config_avformat config_avcodec config_swscale config_swresample no_config_tests" "$QT_AV_DIR/QtAV.pro"
make -j4