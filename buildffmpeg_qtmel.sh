#!/bin/bash
# Builds a specific version of ffmpeg which works with QtMEL.

TAR_FILE="$1"
BUILD_DIR="$2"


if [ ! -e "$BUILD_DIR" ] ; then
	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"
	tar -jxf "$TAR_FILE"
fi

cd "$BUILD_DIR/ffmpeg-git-14d94a1"
./configure --prefix="$BUILD_DIR"/install
make -j4 install