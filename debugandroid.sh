#!/bin/bash

HOME_TAMINO="/home/tamino"
PROJECT_PATH="$HOME_TAMINO/Dokumente/Projekte/fairytale/buildandroidarmeabi-v7a/libs/armeabi-v7a/"
ls "$PROJECT_PATH"
# | grep gustav #
"$HOME_TAMINO/Android/Sdk/platform-tools/adb" logcat | "$HOME_TAMINO/android-ndk-r12b"/ndk-stack -sym "$PROJECT_PATH"