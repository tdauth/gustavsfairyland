# Gustav's Fairyland
Gustav's Fairyland is a simple puzzle game on time where the player has to choose the right clip to construct a custom fairytale which is narrated by the famous composer Gustav Mahler.

It has been created by Tamino Dauth and Carsten Thomas.

## Dependencies
The game depends on several libraries on Fedora:
* qt5-qtbase-gui >= 5.5.1
* qt5-qtmultimedia >= 5.5.1
* qt5-qtsvg >= 5.5.1
* qt5-qttranslations >= 5.5.1

## Installation

### Compilation instructions

Create a binary directory and run:
`cmake <path to the fairytale folder>`

Now compile everything in the binary directory:
`make -j4`

Alternatively you can use one of the scripts:
`bash buildlinux_debug.sh`
`bash buildlinux_release.sh`

For Linux and Windows:
Just use cpack in the build directory:
`cpack .`

It creates an RPM and a DEB package and for Windows a NSIS package.

On Windows 7 the program has been compiled using the MinGW distributed with Qt.
The shell path in CMake has to stay clear.
Unfortunately there is only a 32 MinGW Bit compiler.
Generator: Code Blocks - MinGW Makefiles
CPack creates an NSIS based Windows installer.

For Android:
Download SDK and NDK.
Use the corresponding CMake module.
Add the path to "android" to your PATH ./Android/Sdk/tools/

Use the following CMake modules for Android deployment:
https://github.com/taka-no-me/android-cmake
https://github.com/LaurentGomila/qt-android-cmake/

There is a Bash script "buildandroid.sh" which does everything as long a virtual Android device is installed properly
and the paths are correct.

The program zipalign might not be found for signing the app.
It is in /home/tamino/Android/Sdk/build-tools/24.0.1/zipalign but expected in
/home/tamino/Android/Sdk/build-tools/zipalign
Just do:
`ln -s /home/tamino/Android/Sdk/build-tools/24.0.1/zipalign /home/tamino/Android/Sdk/build-tools/zipalign`

Fix x86 32 Bit Android arch for the release build (apparently Android CMake uses the wrong toolchain name):
`ln -s /home/tamino/android-ndk-r12b/toolchains/x86-4.9/ /home/tamino/android-ndk-r12b/toolchains/i686-linux-android-4.9`

The Android build uses QtAV for playing videos on Android devices since QtMultimediaWidgets are not supported on Android.
Therefore ffmpeg and QtAV have to be build for Android too which happens in the build script "buildqtavforandroid.sh".

If you want to build an Android version for Qt manually, use the script "buildqt.sh".

Use the following command to get debugging output:
/home/tamino/Android/Sdk/platform-tools/adb logcat

Android debugging:
https://wiki.qt.io/How-to-debug-Qt-applications-on-Android-device

Add udev rules for the device:
https://developer.android.com/studio/run/device.html

Install app on the current device (this is done automatically by the CMake module on building):
`./adb -d install -r /home/tamino/Downloads/gustavsfairyland.apk`

Make sure the codec is supported on Android:
https://developer.android.com/guide/appendix/media-formats.html

Audio compression:
Run the Bash script "compressaudio.sh".

Video compression:
Run the Bash script "compressvideos.sh" and make sure it creates videos which can be played on Android as well.

Set the option USE_COMPRESSED_FILES in CMake and it will install the compressed files instead.

There is several Bash scripts for building the game using Jenkins. You can adapt the variables in those scripts for your own environment.
For example "buildandroid_x86.sh" builds the game for Android with the x86 architecture and a specified API level.

Contact: tamino@cdauth.eu