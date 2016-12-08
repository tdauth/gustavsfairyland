#!/bin/bash

# This script builds the project QtAV for Android which requires ffmpeg as well. Therefore ffmpeg has to be build as well.
# To build both projects for Android, the Android NDK is required as well as a binary Qt version for Android.

QT_DIR="$1"
ANDROID_PREFIX="$2"
ANDROID_TARGET="$3"
BUILD_TYPE="$4"

export HOME_TAMINO="/home/tamino"

if [ -z "$QT_DIR" ] ; then
	echo "Setting QT_DIR automatically"
	QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0"
fi

if [ -z "$ANDROID_PREFIX" ] ; then
	echo "Setting ANDROID_PREFIX automatically"
	ANDROID_PREFIX="sdk-android-armv7-gcc" # sdk-android-x86
fi

if [ -z "$ANDROID_TARGET" ] ; then
	echo "Setting ANDROID_TARGET automatically"
	ANDROID_TARGET="armv7" # x86
fi

if [ -z "$BUILD_TYPE" ] ; then
	echo "Define BUILD_TYPE as argument 4 (Debug or Release)"
	exit 1
fi

export NDK_ROOT="$HOME_TAMINO/android-ndk-r12b"
export INSTALL_DIR="../installffmpeg"
export PROJECT_DIR=$(pwd)
export QT_PATH="$QT_DIR"

# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d ./build_ffmpeg ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git ./build_ffmpeg
fi

export FFMPEG_VERSION="3.1.5"
export FFSRC="$PROJECT_DIR/ffmpeg-$FFMPEG_VERSION" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
export FFMPEG_PREFIX="$PROJECT_DIR/build_ffmpeg/$ANDROID_PREFIX"

# Download and extract ffmpeg if it does not exist.
# TODO checksums? or rather use a local copy
if [ ! -e "$FFSRC" ] ; then
	wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2"
	wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2.asc"
	gpg --verify "ffmpeg-$FFMPEG_VERSION.tar.bz2.asc" "ffmpeg-$FFMPEG_VERSION.tar.bz2"
	tar -xjf "ffmpeg-$FFMPEG_VERSION.tar.bz2"
fi

# Overwrite custom Android configuration with the correct NDK path
cp -f "$PROJECT_DIR/config-android.sh" ./build_ffmpeg
# TODO specify NDK_ROOT in file "config-android.sh" as well as static build
cd ./build_ffmpeg
./build_ffmpeg.sh android "$ANDROID_TARGET"

# Change back to the project dir
cd ..

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d ./qtav ]; then
	#rm -rf ./qtav
	git clone https://github.com/wang-bin/QtAV.git ./qtav
fi

cd ./qtav
git submodule update --init
#git checkout tags/v1.11.0

# The user.conf file can have user defined values.
cp -f "$PROJECT_DIR/user.conf" ./
# Add debug and release targets if available.
echo "" >> ./user.conf
echo "CONFIG += debug_and_release" >> ./user.conf

# Change back to the project dir
cd ..

# Always clean the build directory to avoid old stuff.
if [ ! -d ./buildqtav ]; then
	#rm -rf ./buildqtav
	mkdir ./buildqtav
fi

cd ./buildqtav

#export CPATH="$PROJECT_DIR/ffmpeg-3.1.1-android/include/":openal_path/include:$CPATH
#export LIBRARY_PATH="$PROJECT_DIR/ffmpeg-3.1.1-android/lib/armv7":openal_path/lib:$LIBRARY_PATH
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
# += staticlib to build itself statically
# "CONFIG += static_ffmpeg static_openal"
# Needs the library paths for -lswresample and -lavresample
# -L$FFMPEG_LIB_DIR/
# -r "CONFIG+=recheck"
# "CONFIG += no_config_tests" NOTE Don't use this, it leads to errors.
# Take off config_vaapi for ARM?
# "LIBS+=-L$QT_PATH/lib" don't use this and don't create symbolic links
# QMAKE_LIBDIR+=$FFMPEG_LIB_DIR
# If the error "Error: libavresample or libswresample is required" appears, use this:
# https://github.com/wang-bin/QtAV/issues/744
# Add the options "CONFIG+=config_avutil config_avformat config_avcodec config_swscale config_swresample" to the user.conf file if "CONFIG += no_config_tests" is used.
"$QT_PATH/bin/qmake" -Wall "LIBS += -L$FFMPEG_LIB_DIR -lavresample -lswresample" "INCLUDE += -I$FFMPEG_INCLUDE_DIR" ../qtav/QtAV.pro
make -j4 # "${BUILD_TYPE,,}" TODO debug target does not exist