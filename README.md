# Gustav's Fairyland
Gustav's Fairyland is a simple puzzle game on time where the player has to choose the right clip to construct a custom fairytale which is narrated by the famous composer Gustav Mahler.

It has been created by Tamino Dauth and Carsten Thomas.

## Automatic Build with TravisCI


## Manual Build

### Linux
Run one of the following commands:
`cd ./scripts && bash ./buildlinux_debug.sh`
`cd ./scripts && bash ./buildlinux_release.sh`

#### Fedora
The game depends on several libraries on Fedora:
* qt5-qtbase-gui >= 5.5.1
* qt5-qtmultimedia >= 5.5.1
* qt5-qtsvg >= 5.5.1
* qt5-qttranslations >= 5.5.1

### Windows
On Windows 7 the program has been compiled using the MinGW distributed with Qt.
The shell path in CMake has to stay clear.
Unfortunately there is only a 32 MinGW Bit compiler.
Generator: Code Blocks - MinGW Makefiles
CPack creates an NSIS based Windows installer.

### Android
* Download Android SDK and NDK.
* Download the Qt Open Source release for the corresponding Android version.
* Add the path to "android" to your PATH ./Android/Sdk/tools/
* Make sure that the paths in the Bash scripts are set properly (`scripts/buildandroid.sh`, `scripts/buildqtavforandroid.sh` and `scripts/config-android.sh`).
* Go to the `scripts` folder: `cd ./scripts`.
* Execute one of the following commands: `bash ./build_arm_release.sh` or `bash ./build_x86_release.sh`

Use the following CMake module for Android deployment with Qt:
[Qt Android CMake](https://github.com/LaurentGomila/qt-android-cmake/)

There is a Bash script `src/buildandroid.sh` which does everything as long a virtual Android device is installed properly
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

### Software Architecture
The class [fairytale](./src/fairytale.h) represents the main window and main application.

### Clips
The clips consist of video and audio files.
These files have to be converted into different file formats for the different platforms.
This is done by running different Bash scripts.
Set the option `USE_COMPRESSED_FILES` in CMake and it will install the compressed files instead.
It should always be enabled when creating a release build.

Audio compression:
`cd ./scripts && bash ./compressaudio.sh`

Video compression:
`cd ./scripts && bash ./compressvideos.sh`

### Jenkins
There is several Bash scripts for building the game using Jenkins.
You can adapt the variables in those scripts for your own environment.
For the Linux release execute this command:
`cd ./scripts && bash ./jenkins_linux_release.sh`

It creates a TBZ2, an RPM and a DEB package.