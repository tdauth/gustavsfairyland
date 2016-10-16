export HOME_TAMINO="/home/tamino"

export ANDROID_ABI="armeabi-v7a" # x86
export ANDROID_TOOLCHAIN_NAME="arm-linux-androideabi-4.9" # "x86-4.9"
export ANDROID_API_LEVEL="23"
export QT_DIR="$HOME_TAMINO/Qt/5.7/android_armv7" # "$HOME_TAMINO/Qt5.7.0/5.7/android_x86/"
export ANDROID_PREFIX="sdk-android-armv7-gcc" # sdk-android-x86 sdk-android-armv7-gcc
export ANDROID_TARGET="armv7" # x86
export QTAV_LIB_DIR="lib_android_arm" # lib_android_x86

bash -x ./buildandroid.sh