# Gustav's Fairyland
[Gustav's Fairyland](https://gustavsfairyland.wordpress.com/) is a simple puzzle game on time where the player has to choose the right clip to construct a custom fairytale which is narrated by the famous composer [Gustav Mahler](https://en.wikipedia.org/wiki/Gustav_Mahler).

The game has been created by Tamino Dauth and Carsten Thomas.
We have recorded the video clips with the permission of the actors we filmed.
The music composed by Gustav Mahler as well as the graphics were freely available on the Internet.

## Automatic Build with TravisCI on Linux
[![Build Status](https://travis-ci.org/tdauth/gustavsfairyland.svg?branch=master)](https://travis-ci.org/tdauth/gustavsfairyland)

## Automatic Build with AppVeyor on Windows
[Latest Build](https://ci.appveyor.com/project/tdauth/gustavsfairyland)

## Manual Build

### Linux
Run one of the following commands:
`cd ./scripts && bash ./buildlinux_debug.sh`
`cd ./scripts && bash ./buildlinux_release.sh`

It should use the local directory `./clips` when CMAKE_BUILD_TYPE is set to `Debug`.
This helps you to update the clips locally for testing.

#### Fedora
The game depends on several libraries on Fedora:
* qt5-qtbase-gui >= 5.5.1
* qt5-qtmultimedia >= 5.5.1
* qt5-qtsvg >= 5.5.1
* qt5-qttranslations >= 5.5.1
* gstreamer1-libav >= 1.12.4-1 (only if the videos cannot be played)

### Windows
On Windows 7 the program has been compiled using the MinGW distributed with Qt.
The shell path in CMake has to stay clear.
Unfortunately there is only a 32 MinGW Bit compiler.
Generator: Code Blocks - MinGW Makefiles
CPack creates an NSIS based Windows installer.

### Android
* Download Android SDK and NDK.
* Download the Qt Open Source release for the corresponding Android version.
* Add the path to "android" to your PATH "$HOME/Android/Sdk/tools/".
* Make sure that the paths in the Bash scripts are set properly (`scripts/buildandroid.sh`, `scripts/buildqtavforandroid.sh` and `scripts/config-android.sh`).
* Go to the `scripts` folder: `cd ./scripts`.
* Execute one of the following commands: `bash ./build_arm_release.sh` or `bash ./build_x86_release.sh`

Use the following CMake module for Android deployment with Qt:
[Qt Android CMake](https://github.com/LaurentGomila/qt-android-cmake/)

There is a Bash script `scripts/buildandroid.sh` which does everything as long a virtual Android device is installed properly
and the paths are correct.

The program zipalign might not be found for signing the app.
It is in `"$HOME/Android/Sdk/build-tools/24.0.1/zipalign` but expected in `"$HOME/Android/Sdk/build-tools/zipalign"`.
Just do:
`ln -s "$HOME/Android/Sdk/build-tools/24.0.1/zipalign" "$HOME/Android/Sdk/build-tools/zipalign"`

Fix x86 32 Bit Android arch for the release build (apparently Android CMake uses the wrong toolchain name):
`ln -s "$HOME/android-ndk-r12b/toolchains/x86-4.9/" "$HOME/android-ndk-r12b/toolchains/i686-linux-android-4.9"`

The Android build uses QtAV for playing videos on Android devices since QtMultimediaWidgets are not supported on Android.
Therefore ffmpeg and QtAV have to be build for Android too which happens in the build script `scripts/buildqtavforandroid.sh`.

If you want to build an Android version for Qt manually, use the script `scripts/buildqt.sh`.

Use the following command to get debugging output:
```
adb logcat
```

Install app on the current device (this is done automatically by the CMake module on building):
`adb -d install -r gustavsfairyland.apk`

More information:
* [Android debugging with Qt](https://wiki.qt.io/How-to-debug-Qt-applications-on-Android-device)
* [Add udev rules for the device](https://developer.android.com/studio/run/device.html)

Make sure the codec is supported on Android:
https://developer.android.com/guide/appendix/media-formats.html

## Software Architecture
The class [fairytale](./src/fairytale.h) represents the main window and main application.

### Game Modes
The abstract class [GameMode](./src/gamemode.h) allows you to implement custom game modes.
All game modes must be added when the game is started.
There is always a default game mode for a quick start.
Other than that, the game mode can be choosen before starting a game.

## Clips
The clips consist of video and audio files.
These files have to be converted into different file formats for the different platforms: Linux, Android and Windows.
This is done by running different Bash scripts.
Set the option `USE_COMPRESSED_FILES` in CMake and it will install the compressed files instead.
It should always be enabled when creating a release build.

Audio compression:
`cd ./scripts && bash ./compressaudio.sh`

Video compression:
`cd ./scripts && bash ./compressvideos.sh`

## Jenkins
There is several Bash scripts for building the game using Jenkins.
You can adapt the variables in those scripts for your own environment.
For the Linux release execute this command:
`cd ./scripts && bash ./jenkins_linux_release.sh`

It creates a TBZ2, an RPM and a DEB package.