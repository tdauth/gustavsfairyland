if [ -d "./buildqt5" ] ; then
	rm -rf "./buildqt5"
fi

mkdir ./buildqt5
git clone git://code.qt.io/qt/qt5.git qt5
cd qt5
# Not using all modules reduces the build time.
perl init-repository -f --module-subset=default,-qtwebkit,-qtwebkit-examples,-qtwebengine
export HOME_TAMINO="/home/tamino"
export JAVA_HOME=/usr/java/default/ # You need a JDK (version 1.6+, required 32bit) and you should also have this in your PATH.
export ANDROID_HOME="$HOME_TAMINO/Android/Sdk"
export PATH="$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"
export ANDROID_SDK_ROOT="$ANDROID_HOME"
export ANDROID_BUILD_TOOLS_REVISION="24.0.1"
./configure -confirm-license -developer-build -opensource -xplatform android-g++ -nomake tests -nomake examples -android-ndk "$HOME_TAMINO/android-ndk-r12b" -android-sdk "$ANDROID_SDK_ROOT" -android-ndk-host linux-x86_64 -android-toolchain-version 4.9 -skip qttranslations -skip qtwebkit -skip qtserialport -skip qtwebkit-examples -no-warnings-are-errors
make -j4