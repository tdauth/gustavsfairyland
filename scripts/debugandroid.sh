#!/bin/bash

HOME_TAMINO="/home/tamino"
PROJECT_PATH="$HOME_TAMINO/Dokumente/Projekte/fairytale/buildandroidarmeabi-v7a/libs/armeabi-v7a/"
QTAV_FFMPEG_LOG=debug
ls "$PROJECT_PATH"
#"$HOME_TAMINO/Android/Sdk/platform-tools/adb" logcat
"$HOME_TAMINO/Android/Sdk/platform-tools/adb" logcat | grep 'gustav\|QtAv'
#"$HOME_TAMINO/Android/Sdk/platform-tools/adb" logcat | "$HOME_TAMINO/android-ndk-r12b"/ndk-stack -sym "$PROJECT_PATH"