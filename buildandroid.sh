if [ ! -d "./buildandroid" ] ; then
	mkdir ./buildandroid
fi
echo "$HOME"
export PATH="$HOME/Android/Sdk/tools/:$PATH"
export ANDROID_HOME="$HOME/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$PATH"
export PATH="$ANDROID_HOME/platform-tools:$PATH"
cd ./buildandroid
cmake -DCMAKE_MODULE_PATH="../cmake;/home/tamino/Qt5.7.0/5.7/android_armv7/lib/cmake" -DCMAKE_TOOLCHAIN_FILE=../cmake/android.toolchain.cmake -DANDROID_NDK=/home/tamino/android-ndk-r12b -DCMAKE_BUILD_TYPE=Debug -DANDROID_ABI="armeabi"  -DANDROID_NATIVE_API_LEVEL="24" ../
cmake --build .
