if [ ! -d "./buildandroid" ] ; then
	mkdir ./buildandroid
fi
echo "$HOME"
export PATH="$HOME/Android/Sdk/tools/:$PATH"
export ANDROID_HOME="$HOME/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$PATH"
export PATH="$ANDROID_HOME/platform-tools:$PATH"
cd ./buildandroid
# If build type Debug is used CMAKE_GDBSERVER has to specified for the command android_create_apk()
cmake -DCMAKE_MODULE_PATH="../cmake" -DCMAKE_PREFIX_PATH="/home/tamino/Qt5.7.0/5.7/android_armv7/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK=/home/tamino/android-ndk-r12b -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi"  -DANDROID_NATIVE_API_LEVEL="24" -DQT_QMAKE_EXECUTABLE="/home/tamino/Qt5.7.0/5.7/android_armv7/bin/qmake" ../
cmake --build .
