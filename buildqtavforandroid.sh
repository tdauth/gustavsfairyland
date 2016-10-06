#!/bin/bash

QT_DIR="$1"
ANDROID_PREFIX="$2"
ANDROID_TARGET="$3"

export HOME_TAMINO="/home/tamino"

if [ -z "$QT_DIR" ] ; then
	echo "Setting QT_DIR automatically"
	QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0"
fi

if [ -z "$ANDROID_PREFIX" ] ; then
	echo "Setting ANDROID_PREFIX automatically"
	ANDROID_PREFIX="sdk-android-armv7" # sdk-android-x86
fi

if [ -z "$ANDROID_TARGET" ] ; then
	echo "Setting ANDROID_TARGET automatically"
	ANDROID_TARGET="armv7" # x86
fi

export NDK_ROOT="$HOME_TAMINO/android-ndk-r12b"
export INSTALL_DIR="../installffmpeg"
export PROJECT_DIR=$(pwd)
#export QT_PATH="$HOME_TAMINO/Qt5.7.0"
export QT_PATH="$QT_DIR"

# TODO build ffmpeg with Android compiler
# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d ./build_ffmpeg ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git ./build_ffmpeg
fi

export FFSRC="$PROJECT_DIR/ffmpeg-3.1.4/" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
export FFMPEG_PREFIX="$PROJECT_DIR/build_ffmpeg/$ANDROID_PREFIX/"

# Download and extract ffmpeg if it does not exist.
# TODO checksums? or rather use a local copy
if [ ! -e "$FFSRC" ] ; then
	wget http://ffmpeg.org/releases/ffmpeg-3.1.4.tar.bz2
	tar -xjf ffmpeg-3.1.4.tar.bz2
fi

# Overwrite custom Android configuration with the correct NDK path
cp -f "$PROJECT_DIR/config-android.sh" ./build_ffmpeg
# TODO specify NDK_ROOT in file "config-android.sh" as well as static build
cd ./build_ffmpeg
./build_ffmpeg.sh android "$ANDROID_TARGET"

# Change back to the project dir
cd ..

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ -d ./qtav ]; then
	rm -rf ./qtav
fi

git clone https://github.com/wang-bin/QtAV.git ./qtav
cd ./qtav
git submodule update --init

cd ..

if [ -d ./buildqtav ]; then
	rm -rf ./buildqtav
fi

mkdir ./buildqtav
cd ./buildqtav
#export CPATH="$PROJECT_DIR/ffmpeg-3.1.1-android/include/":openal_path/include:$CPATH
#export LIBRARY_PATH="$PROJECT_DIR/ffmpeg-3.1.1-android/lib/armv7":openal_path/lib:$LIBRARY_PATH
export CPATH="$FFMPEG_PREFIX/include/:$CPATH"
export FFMPEG_LIB_DIR="$FFMPEG_PREFIX/lib"
export LIBRARY_PATH="$FFMPEG_LIB_DIR:$LIBRARY_PATH"
export LD_LIBRARY_PATH="$FFMPEG_LIB_DIR:$LD_LIBRARY_PATH"
export ANDROID_NDK_ROOT="$NDK_ROOT"

# for f in "$FFMPEG_PREFIX/include/"* ; do
# 	target="$QT_PATH/include"
# 	echo "Creating link for $f to $target"
# 	ln -fs "$f" "$target"
# done
# 
# for f in "$FFMPEG_LIB_DIR/"* ; do
# 	target="$QT_PATH/lib"
# 	echo "Creating link for $f to $target"
# 	ln -fs "$f" "$target"
# done

echo "LIBRARY_PATH: $LIBRARY_PATH"
echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "ls $FFMPEG_LIB_DIR/:"
ls "$FFMPEG_LIB_DIR/"
echo "Building QtAV from: $(pwd)"
echo "Running qmake: \"$QT_PATH/bin/qmake\""
# "$QT_PATH/5.7/android_x86/bin/qmake"
# += staticlib to build itself statically
# "CONFIG += static_ffmpeg static_openal"
# Needs the library paths for -lswresample and -lavresample
# -L$FFMPEG_LIB_DIR/
# -r "CONFIG+=recheck"
# "CONFIG += no_config_tests" NOTE Don't use this, it leads to errors.
# Take off config_vaapi  for ARM?
# "LIBS+=-L$QT_PATH/lib" don't use this and don't create symbolic links
"$QT_PATH/bin/qmake" -Wall "LIBS+=-L$FFMPEG_LIB_DIR" ../qtav/QtAV.pro
make -j4