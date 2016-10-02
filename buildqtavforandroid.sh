#!/bin/bash

# TODO build ffmpeg with Android compiler
# https://github.com/wang-bin/build_ffmpeg/wiki
if [ ! -d ./build_ffmpeg ] ; then
	git clone https://github.com/wang-bin/build_ffmpeg.git ./build_ffmpeg
fi
cd ./build_ffmpeg
export NDK_ROOT="$HOME/android-ndk-r12b"
export INSTALL_DIR="../installffmpeg"
export FFSRC="$HOME/Dokumente/Projekte/fairytale/ffmpeg-3.1.4/" #/path/to/ffmpeg # if no ffmpeg source fold under this dir
./build_ffmpeg.sh android x86

# https://github.com/wang-bin/QtAV/wiki/Build-QtAV
if [ ! -d ./qtav ]; then
	git clone https://github.com/wang-bin/QtAV.git ./qtav
fi
cd ./qtav
git pull
git submodule update --init

cd ..

if [ -d ./buildqtav ]; then
	echo "Deleting ./buildqtav"
	rm -rf ./buildqtav
fi

mkdir ./buildqtav
cd ./buildqtav
#export CPATH="$HOME/Dokumente/Projekte/fairytale/ffmpeg-3.1.1-android/include/":openal_path/include:$CPATH
#export LIBRARY_PATH="$HOME/Dokumente/Projekte/fairytale/ffmpeg-3.1.1-android/lib/armv7":openal_path/lib:$LIBRARY_PATH
export CPATH="$HOME/Dokumente/Projekte/fairytale/build_ffmpeg/sdk-android-x86/include/":$CPATH
export LIBRARY_PATH="$HOME/Dokumente/Projekte/fairytale/build_ffmpeg/sdk-android-x86/lib/":$LIBRARY_PATH
export LD_LIBRARY_PATH="$HOME/Dokumente/Projekte/fairytale/build_ffmpeg/sdk-android-x86/lib/":$LD_LIBRARY_PATH
export ANDROID_NDK_ROOT="$HOME/android-ndk-r12b"

for f in "$HOME/Dokumente/Projekte/fairytale/build_ffmpeg/sdk-android-x86/include/"* ; do
	echo "Creating link for $f"
	target="$HOME/Qt5.7.0/5.7/android_x86/include"

	if [ ! -e "$target" ] ; then
		ln -s "$f" "$target"
	fi
done

for f in "$HOME/Dokumente/Projekte/fairytale/build_ffmpeg/sdk-android-x86/lib/"* ; do
	echo "Creating link for $f"
	target="$HOME/Qt5.7.0/5.7/android_x86/lib"

	if [ ! -e "$target" ] ; then
		ln -s "$f" "$target"
	fi
done

# "$HOME/Qt5.7.0/5.7/android_x86/bin/qmake"
"$HOME/Qt5.7.0/5.7/android_x86/bin/qmake" ../qtav/QtAV.pro -config no_config_tests -r "CONFIG+=recheck"
#make -j4