#!/bin/bash

# This script builds the project QtAV for Android which requires ffmpeg as well. Therefore ffmpeg has to be build as well.
# To build both projects for Android, the Android NDK is required as well as a binary Qt version for Android.

BUILD_TYPE="$1"

export HOME_TAMINO="/home/tamino"

if [ -z "$BUILD_TYPE" ] ; then
	echo "Define BUILD_TYPE as argument 4 (Debug or Release)"
	exit 1
fi

export INSTALL_DIR="../installffmpeg"
export PROJECT_DIR=$(pwd)
export QT_PATH="$QT_DIR"

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d ./qtav ]; then
	#rm -rf ./qtav
	git clone https://github.com/wang-bin/QtAV.git ./qtav
fi

cd ./qtav
git submodule update --init
#git checkout tags/v1.11.0

# The user.conf file can have user defined values.
cp -f "$PROJECT_DIR/userlinux.conf" ./user.conf
cp -f "$PROJECT_DIR/userlinux.conf" ./src/user.conf

# Change back to the project dir
cd ..

# Always clean the build directory to avoid old stuff.
if [ ! -d ./buildqtav ]; then
	#rm -rf ./buildqtav
	mkdir ./buildqtav
fi

cd ./buildqtav

export FFMPEG_PREFIX="/usr"
export FFMPEG_INCLUDE_DIR="$FFMPEG_PREFIX/include/ffmpeg"
export FFMPEG_LIB_DIR="$FFMPEG_PREFIX/lib/ffmpeg"
export CPATH="$FFMPEG_INCLUDE_DIR/:$CPATH"
export LIBRARY_PATH="$FFMPEG_LIB_DIR:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$FFMPEG_LIB_DIR:$LD_LIBRARY_PATH"

echo "LIBRARY_PATH: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
#echo "ls -lha $FFMPEG_LIB_DIR/:"
#ls -lha "$FFMPEG_LIB_DIR/"
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
make -j4 #  "${BUILD_TYPE,,}" TODO debug target does not exist although in the Makefile!