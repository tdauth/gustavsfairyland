#!/bin/bash

# This script builds the project QtAV for Android which requires ffmpeg as well. Therefore ffmpeg has to be build as well.
# To build both projects for Android, the Android NDK is required as well as a binary Qt version for Android.

BUILD_TYPE="$1"
PROJECT_DIR="$2"
PROJECT_BIN_DIR="$3"
QT_DIR="$4"
ANDROID_ABI="$5"
ANDROID_TARGET="$6"
ANDROID_API_LEVEL="$7"
ANDROID_NDK="$8"

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
	echo "Define QT_DIR as argument 4"
	exit 1
fi

if [ -z "$ANDROID_ABI" ] ; then
	echo "Define ANDROID_ABI as argument 5"
	exit 1
fi

if [ -z "$ANDROID_TARGET" ] ; then
	echo "Define ANDROID_TARGET as argument 6"
	exit 1
fi

if [ -z "$ANDROID_API_LEVEL" ] ; then
	echo "Define ANDROID_API_LEVEL as argument 7"
	exit 1
fi

if [ -z "$ANDROID_NDK" ] ; then
	echo "Define ANDROID_NDK as argument 8"
	exit 1
fi

export NDK_ROOT="$ANDROID_NDK"
export INSTALL_DIR="../installffmpeg"
export QT_PATH="$QT_DIR"

# Work in the binary dir.
cd "$PROJECT_BIN_DIR"

export FFMPEG_DIR="$PROJECT_BIN_DIR/build_ffmpeg"

# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d "$FFMPEG_DIR" ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git "$FFMPEG_DIR"
fi

export FFMPEG_VERSION="3.3"
export FFSRC="$PROJECT_BIN_DIR/ffmpeg-$FFMPEG_VERSION" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
export FFMPEG_PREFIX="$FFMPEG_DIR/sdk-android-gcc"

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
./avbuild.sh android "$ANDROID_TARGET"

if [ ! "$?" -eq "0" ] ; then
	echo "Failed to build ffmpeg for Android."
	exit 1
fi

export QT_AV_DIR="$PROJECT_BIN_DIR/qtav"

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d "$QT_AV_DIR" ]; then
	git clone https://github.com/wang-bin/QtAV.git "$QT_AV_DIR"
fi

cd "$QT_AV_DIR"
git submodule update --init

export QT_AV_BUILD_DIR="$PROJECT_BIN_DIR/buildqtav"

# Always clean the build directory to avoid old stuff.
if [ ! -d "$QT_AV_BUILD_DIR" ]; then
	mkdir "$QT_AV_BUILD_DIR"
fi

cd "$QT_AV_BUILD_DIR"

export FFMPEG_INCLUDE_DIR="$FFMPEG_PREFIX/include"
export FFMPEG_LIB_DIR="$FFMPEG_PREFIX/lib/$ANDROID_ABI"

echo "ffmpeg include dir:"
echo "ls -lha $FFMPEG_INCLUDE_DIR/:"
ls -lha "$FFMPEG_INCLUDE_DIR/"

echo "ffmpeg link dir:"
echo "ls -lha $FFMPEG_LIB_DIR/:"
ls -lha "$FFMPEG_LIB_DIR/"

# CMake build:
cmake "$QT_AV_DIR" \
-DCMAKE_SYSTEM_NAME=Android \
-DCMAKE_SYSTEM_VERSION="$ANDROID_API_LEVEL" \
-DCMAKE_ANDROID_ARCH_ABI="$ANDROID_ABI" \
-DCMAKE_ANDROID_NDK="$ANDROID_NDK" \
-DCMAKE_ANDROID_STL_TYPE=gnustl_static \
-DCMAKE_VERBOSE_MAKEFILE=on \
-DCMAKE_PREFIX_PATH="$QT_DIR/lib/cmake;$FFMPEG_LIB_DIR;$FFMPEG_INCLUDE_DIR" \
-DCMAKE_LIBRARY_PATH="$FFMPEG_LIB_DIR" \
-DCMAKE_INCLUDE_PATH="$FFMPEG_INCLUDE_DIR" \
-DCMAKE_CXX_FLAGS="-I$FFMPEG_INCLUDE_DIR -I$QT_DIR/include/QtAndroidExtras/ -L$FFMPEG_LIB_DIR -L$QT_DIR/lib -lQt5AndroidExtras" \
-DBUILD_EXAMPLES=0 \
-DBUILD_TESTS=0
make #-j4