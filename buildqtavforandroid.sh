#!/bin/bash

export HOME_TAMINO="/home/tamino"
export NDK_ROOT="$HOME_TAMINO/android-ndk-r12b"
export INSTALL_DIR="../installffmpeg"
export PROJECT_DIR=$(pwd)
export QT_PATH="$HOME_TAMINO/Qt5.7.0"

# TODO build ffmpeg with Android compiler
# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d ./build_ffmpeg ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git ./build_ffmpeg
fi

export FFSRC="$PROJECT_DIR/ffmpeg-3.1.4/" #/path/to/ffmpeg # if no ffmpeg source fold under this dir

# Download and extract ffmpeg if it does not exist.
# TODO checksums? or rather use a local copy
if [ ! -e "$FFSRC" ] ; then
	wget http://ffmpeg.org/releases/ffmpeg-3.1.4.tar.bz2
	tar -xjf ffmpeg-3.1.4.tar.bz2
fi

cd ./build_ffmpeg
./build_ffmpeg.sh android x86

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d ./qtav ]; then
	git clone https://github.com/wang-bin/QtAV.git ./qtav
	cd ./qtav
	git submodule update --init
	cd ..
fi
cd ./qtav
git submodule update --init

cd ..

if [ -d ./buildqtav ]; then
	echo "Deleting ./buildqtav"
	rm -rf ./buildqtav
fi

mkdir ./buildqtav
cd ./buildqtav
#export CPATH="$PROJECT_DIR/ffmpeg-3.1.1-android/include/":openal_path/include:$CPATH
#export LIBRARY_PATH="$PROJECT_DIR/ffmpeg-3.1.1-android/lib/armv7":openal_path/lib:$LIBRARY_PATH
export CPATH="$PROJECT_DIR/build_ffmpeg/sdk-android-x86/include/":$CPATH
export LIBRARY_PATH="$PROJECT_DIR/build_ffmpeg/sdk-android-x86/lib/":$LIBRARY_PATH
export LD_LIBRARY_PATH="$PROJECT_DIR/build_ffmpeg/sdk-android-x86/lib/":$LD_LIBRARY_PATH
export ANDROID_NDK_ROOT="$NDK_ROOT"

for f in "$PROJECT_DIR/build_ffmpeg/sdk-android-x86/include/"* ; do
	echo "Creating link for $f"
	target="$QT_PATH/5.7/android_x86/include"

	if [ ! -e "$target" ] ; then
		ln -s "$f" "$target"
	fi
done

for f in "$PROJECT_DIR/build_ffmpeg/sdk-android-x86/lib/"* ; do
	echo "Creating link for $f"
	target="$QT_PATH/5.7/android_x86/lib"

	if [ ! -e "$target" ] ; then
		ln -s "$f" "$target"
	fi
done

# "$QT_PATH/5.7/android_x86/bin/qmake"
"$QT_PATH/5.7/android_x86/bin/qmake" ../qtav/QtAV.pro -config no_config_tests -r "CONFIG+=recheck" "LIBS+=-L$PROJECT_DIR/build_ffmpeg/sdk-android-x86/lib/"
#make -j4