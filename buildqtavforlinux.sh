#!/bin/bash

# This script builds the project QtAV for Linux which requires ffmpeg as well. Therefore ffmpeg has to be build as well.

BUILD_TYPE="$1"
PROJECT_DIR="$2"
PROJECT_BIN_DIR="$3"

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

export HOME_TAMINO="/home/tamino"
export INSTALL_DIR="$PROJECT_BIN_DIR/installffmpeg"

# Work in the binary dir.
cd "$PROJECT_BIN_DIR"

# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d ./build_ffmpeg ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git ./build_ffmpeg
fi

export FFMPEG_VERSION="3.1.5"
export FFSRC="$PROJECT_BIN_DIR/ffmpeg-$FFMPEG_VERSION" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
export FFMPEG_PREFIX="$PROJECT_BIN_DIR/ffmpeg-$FFMPEG_VERSION"

# Download and extract ffmpeg if it does not exist.
# TODO checksums? or rather use a local copy
if [ ! -e "$FFSRC" ] ; then
	wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2"
	wget "http://ffmpeg.org/releases/ffmpeg-$FFMPEG_VERSION.tar.bz2.asc"
	gpg --verify "ffmpeg-$FFMPEG_VERSION.tar.bz2.asc" "ffmpeg-$FFMPEG_VERSION.tar.bz2"
	tar -xjf "ffmpeg-$FFMPEG_VERSION.tar.bz2"
fi

cd "./ffmpeg-$FFMPEG_VERSION"

# Build FFMPEG with debug mode: https://lists.libav.org/pipermail/ffmpeg-user/2009-January/018741.html
if [ "$BUILD_TYPE" = "Debug" ] ; then
	./configure --enable-shared --disable-static --disable-optimizations --disable-mmx --disable-stripping
else
	./configure
fi

make -j4


# Change back to the project dir
cd "$PROJECT_BIN_DIR"

export QT_AV_DIR="$PROJECT_BIN_DIR/qtav"

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d "$QT_AV_DIR" ]; then
	#rm -rf ./qtav
	git clone https://github.com/wang-bin/QtAV.git "$QT_AV_DIR"
fi

cd "$QT_AV_DIR"
git submodule update --init
#git checkout tags/v1.11.0

# The user.conf file can have user defined values.
cp -f "$PROJECT_DIR/userlinux.conf" "$QT_AV_BUILD_DIR/user.conf"
cp -f "$PROJECT_DIR/userlinux.conf" "$QT_AV_BUILD_DIR/src/user.conf"

export QT_AV_BUILD_DIR="$PROJECT_BIN_DIR/buildqtav"

# Always clean the build directory to avoid old stuff.
if [ ! -d "$QT_AV_BUILD_DIR" ]; then
	#rm -rf ./buildqtav
	mkdir "$QT_AV_BUILD_DIR"
fi

cd "$QT_AV_BUILD_DIR"

export FFMPEG_INCLUDE_DIR="$FFMPEG_PREFIX/"
export FFMPEG_LIB_DIR="$FFMPEG_PREFIX/lib/ffmpeg"
export CPATH="$FFMPEG_INCLUDE_DIR/:$CPATH"
export LIBRARY_PATH="$FFMPEG_LIB_DIR:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$FFMPEG_LIB_DIR:$LD_LIBRARY_PATH"

echo "LIBRARY_PATH: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
#echo "ls -lha $FFMPEG_LIB_DIR/:"
#ls -lha "$FFMPEG_LIB_DIR/"
echo "Building QtAV from: $(pwd)"
echo "Running qmake: \"$(which qmake)\""

# If the error "Error: libavresample or libswresample is required" appears, use this:
# https://github.com/wang-bin/QtAV/issues/744
# Add the options "CONFIG+=config_avutil config_avformat config_avcodec config_swscale config_swresample" to the user.conf file if "CONFIG += no_config_tests" is used.
qmake -Wall "LIBS += -L$FFMPEG_LIB_DIR -lavresample -lswresample" "INCLUDE += -I$FFMPEG_INCLUDE_DIR" "$QT_AV_DIR/QtAV.pro"
make -j4 #  "${BUILD_TYPE,,}" TODO debug target does not exist although in the Makefile!