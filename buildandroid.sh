if [ ! -d "./buildandroid" ] ; then
	mkdir ./buildandroid
fi
echo "$HOME"
export PATH="$HOME/Android/Sdk/tools/:$PATH"
export ANDROID_HOME="$HOME/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$PATH"
export PATH="$ANDROID_HOME/platform-tools:$PATH"
export ANT_PATH=$(which ant)
cd ./buildandroid
# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake -DJAVA_HOME="/usr/java/default/" -DANDROID_SDK="$ANDROID_HOME" -DQT_ANDROID_SDK_ROOT="$ANDROID_HOME" -DQT_ANDROID_ANT="$ANT_PATH" -DCMAKE_MODULE_PATH="../cmake" -DCMAKE_PREFIX_PATH="$HOME/Qt5.7.0/5.7/android_x86/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK="$HOME/android-ndk-r12b" -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="x86"  -DANDROID_NATIVE_API_LEVEL="24" ../
cmake --build .
# Manual installation of the packed apk file for the emulator
echo "Installing the APK file"
$ANDROID_HOME/platform-tools/adb install -r ./bin/QtApp-debug.apk
echo "Result: $?"
