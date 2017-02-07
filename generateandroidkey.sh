#!/bin/bash
# http://www.androiddevelopment.org/tag/apk/
# http://stackoverflow.com/questions/4835925/unsigned-apk-can-not-be-installed
"$HOME/android-studio/jre/bin/keytool" -genkey -v -keystore my-release-key.keystore -alias gustavsfairyland -keyalg RSA -validity 10000