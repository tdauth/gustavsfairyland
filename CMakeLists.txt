cmake_minimum_required(VERSION 3.4.3)

# TODO use Hunter for easier dependency resolving
include("cmake/HunterGate.cmake")

HunterGate(
	URL "https://github.com/ruslo/hunter/archive/v0.16.15.tar.gz"
	SHA1 "6974c2150fc0d3b09de3ad1efcbf15d360647ffa"
)

project(gustavsfairyland)

#hunter_add_package(Qt)

if (ANDROID)
	add_definitions(-DANDROID_ICON="@drawable/logo")
	set(ANDROID_ICON "@drawable/logo")
	include("cmake/AddQtAndroidApk.cmake" REQUIRED)
	include_directories("${ANDROID_NDK}/sources/android/native_app_glue/")
endif ()

enable_testing()

add_compile_options(-std=c++11 -Wall)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-DDEBUG=1)
elseif (WIN32)
	# Don't show a console in release mode on Windows: -mwindows
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
endif ()

message(STATUS "Compiling for ${CMAKE_SYSTEM_NAME}")

option(USE_COMPRESSED_FILES "Enable this option to install much smaller videos and sound files. The files need to be generated before using this option by the scripts compressvideos.sh and compressaudio.sh." FALSE)
option(USE_QTAV "Enable this option to use QtAV for playing video files." FALSE)
option(USE_QTMEL "Enable this option to use QtMEL for recording video files." FALSE)
option(USE_VLCQT "Enable this option to use VLC-Qt. Another library to play videos." FALSE)
option(USE_ANDROID_JAVA_PLAYER "Enable this option to use a Java-based video player on Android." FALSE)
option(USE_DOXYGEN "Enable this option to generate an API documentation using Doxygen if it is available." FALSE)

set(gustavsfairyland_VERSION_MAJOR "1")
set(gustavsfairyland_VERSION_MINOR "3")
set(gustavsfairyland_VERSION_PATCH "0")
set(gustavsfairyland_VERSION "${gustavsfairyland_VERSION_MAJOR}.${gustavsfairyland_VERSION_MINOR}.${gustavsfairyland_VERSION_PATCH}")

configure_file(config.h.in config.h ESCAPE_QUOTES)
include_directories("${CMAKE_BINARY_DIR}")

# Validate XML files if possible
find_program(XMLLINT_PATH xmllint)

if (XMLLINT_PATH)
	message(STATUS "Validating XML files.")
	set(XML_FILES
		"${CMAKE_SOURCE_DIR}/clips/gustav.xml"
		"${CMAKE_SOURCE_DIR}/clips/custom.xml"
		"${CMAKE_SOURCE_DIR}/src/test/clips.xml"
	)
	# TODO check against an XML schema as well
	add_custom_target(ValidateXMLFiles ALL ${XMLLINT_PATH} --noout ${XML_FILES} SOURCES ${XML_FILES})
endif ()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Handle .rcc files automatically
set(CMAKE_AUTORCC ON)

# Find all Qt modules which are required
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

# Requires QtAV for playing videos
if (USE_QTAV)
	find_package(Qt5OpenGL REQUIRED) # Required by QtAV

	if (ANDROID)
		find_package(Qt5AndroidExtras REQUIRED) # Required by QtAV
	endif ()
endif ()

# On Windows recording with the webcam is not supported yet by QtMultimedia. Therefore we need an alternative library.
if (USE_QTMEL)
	include(ExternalProject)

	hunter_add_package(OpenCV)
	hunter_add_package(ZLIB)

	find_package(OpenCV REQUIRED)
	find_package(ZLIB REQUIRED)

	add_custom_target(ffmpegQtMEL ALL bash -x buildffmpeg_qtmel.sh "${CMAKE_SOURCE_DIR}/ffmpeg-git-14d94a1.tar.bz2" "${CMAKE_BINARY_DIR}/ffmpeg-git-14d94a1" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" SOURCES "${CMAKE_SOURCE_DIR}/ffmpeg-git-14d94a1.tar.bz2" "${CMAKE_SOURCE_DIR}/buildffmpeg_qtmel.sh")
	set(AV_INCLUDE "${CMAKE_BINARY_DIR}/ffmpeg-git-14d94a1/install/include/")
	set(AV_LIBRARY_DIR "${CMAKE_BINARY_DIR}/ffmpeg-git-14d94a1/install/lib/")

	# Pass $$(FFMPEG_INCLUDE_PATH) $$(OPENCV_INCLUDE_PATH)
	ExternalProject_Add(QtMEL
		DEPENDS ffmpegQtMEL
		GIT_REPOSITORY https://github.com/kibsoft/QtMEL.git
		CONFIGURE_COMMAND FFMPEG_INCLUDE_PATH=${AV_INCLUDE} qmake -Wall "INCLUDEPATH += ${AV_INCLUDE}" "${CMAKE_BINARY_DIR}/QtMEL-prefix/src/QtMEL/qtmel.pro"
		BUILD_COMMAND make
		INSTALL_COMMAND ""
	)

	set(AV_LIBRARIES "libavcodec.a" "libavdevice.a" "libavfilter.a" "libavformat.a" "libavutil.a" "libswresample.a" "libswscale.a" "pthread")
	set(QT_MEL_LIBRARIES "${CMAKE_BINARY_DIR}/QtMEL-prefix/src/QtMEL/libsQt5/libqtmel.so")
	include_directories("${CMAKE_BINARY_DIR}/QtMEL-prefix/src/QtMEL/include/")
	link_directories("${AV_LIBRARY_DIR}")
endif ()

if (USE_VLCQT)
	include(ExternalProject)

	# Requires libVLC (vlc-devel on Fedora) and Qt 5
	ExternalProject_Add(VLCQt
		GIT_REPOSITORY https://github.com/vlc-qt/vlc-qt.git
		INSTALL_COMMAND ""
	)
endif ()

set(fairyland_HEADERS
	src/aboutdialog.h
	src/fairytale.h
	src/floatingclip.h
	src/clip.h
	src/door.h
	src/player.h
	src/iconbutton.h
	src/clippackage.h
	src/clippackagedialog.h
	src/clippackageeditor.h
	src/clipeditor.h
	src/customfairytale.h
	src/customfairytaledialog.h
	src/gamemode.h
	src/gamemodemoving.h
	src/gamemodeoneoutoffour.h
	src/gamemodecreative.h
	src/gamemodesequence.h
	src/iconbutton.h
	src/roomwidget.h
	src/settingsdialog.h
	src/wondialog.h
	src/iconlabel.h
	src/highscores.h
	src/bonusclip.h
	src/gameoverdialog.h
	src/languagedialog.h
	src/clickanimation.h
	src/recorder.h
	src/bonusclipsdialog.h
	src/fairytalesdialog.h
	src/localedialog.h
	src/translatedwidget.h
	src/solutionwidget.h
	src/abstractclip.h
)

set(fairyland_SRCS
	src/aboutdialog.cpp
	src/fairytale.cpp
	src/floatingclip.cpp
	src/clip.cpp
	src/door.cpp
	src/player.cpp
	src/iconbutton.cpp
	src/clippackage.cpp
	src/clippackagedialog.cpp
	src/clippackageeditor.cpp
	src/clipeditor.cpp
	src/customfairytale.cpp
	src/customfairytaledialog.cpp
	src/gamemode.cpp
	src/gamemodemoving.cpp
	src/gamemodeoneoutoffour.cpp
	src/gamemodecreative.cpp
	src/gamemodesequence.cpp
	src/iconbutton.cpp
	src/roomwidget.cpp
	src/settingsdialog.cpp
	src/wondialog.cpp
	src/iconlabel.cpp
	src/highscores.cpp
	src/bonusclip.cpp
	src/gameoverdialog.cpp
	src/languagedialog.cpp
	src/clickanimation.cpp
	src/recorder.cpp
	src/bonusclipsdialog.cpp
	src/fairytalesdialog.cpp
	src/localedialog.cpp
	src/translatedwidget.cpp
	src/solutionwidget.cpp
	src/abstractclip.cpp
)

set(fairyland_UI
	ui/aboutdialog.ui
	ui/mainwindow.ui
	ui/player.ui
	ui/clippackagedialog.ui
	ui/clippackageeditor.ui
	ui/clipeditor.ui
	ui/customfairytale.ui
	ui/settings.ui
	ui/wondialog.ui
	ui/highscores.ui
	ui/gameoverdialog.ui
	ui/languagedialog.ui
	ui/recorder.ui
	ui/bonusclipsdialog.ui
	ui/fairytalesdialog.ui
	ui/localedialog.ui
)

qt5_wrap_ui(fairyland_UI_HEADERS ${fairyland_UI})
include_directories("${CMAKE_BINARY_DIR}/ui")

set(fairyland_TRANSLATIONS
	translations/en.ts
	translations/de.ts
)

set(fairytale_TRANSLATION_SRCS ${fairyland_SRCS} ${fairyland_HEADERS} ${fairyland_UI_HEADERS})
QT5_CREATE_TRANSLATION(fairytale_QM_FILES ${fairytale_TRANSLATION_SRCS} ${fairyland_TRANSLATIONS})

# There is an out of memory allocation on Windows with the Oxygen resources file.
# Besides that you should use oxygen.qrc to find the icons and add them to oxygen_windows.qrc to keep the executable small.
QT5_ADD_RESOURCES(fairyland_RESOURCES_RCC resource.qrc oxygen-icons-png-master/oxygen_windows.qrc)

if (USE_QTAV)
	if (ANDROID)
		set(QTAV_FFMPEG_LIB_DIR "${CMAKE_BINARY_DIR}/build_ffmpeg/${ANDROID_PREFIX}/lib")
	else ()
		set(QTAV_FFMPEG_LIB_DIR "${CMAKE_BINARY_DIR}/ffmpeginstall/lib")
		set(QTAV_LIB_DIR "lib_linux_x86_64")
	endif ()

	set(QTAV_LIBRARIES
		"${QTAV_FFMPEG_LIB_DIR}/libavcodec.so"
		"${QTAV_FFMPEG_LIB_DIR}/libavfilter.so"
		"${QTAV_FFMPEG_LIB_DIR}/libavformat.so"
		"${QTAV_FFMPEG_LIB_DIR}/libavresample.so"
		"${QTAV_FFMPEG_LIB_DIR}/libavutil.so"
		"${QTAV_FFMPEG_LIB_DIR}/libswresample.so"
		"${QTAV_FFMPEG_LIB_DIR}/libswscale.so"
		"${CMAKE_BINARY_DIR}/buildqtav/${QTAV_LIB_DIR}/libQtAV.so"
		"${CMAKE_BINARY_DIR}/buildqtav/${QTAV_LIB_DIR}/libQtAVWidgets.so"
	)

	if (ANDROID)
		# Builds QtAV for Android which is required since Qt5MultimediaWidgets is not supported on Android.
		add_custom_target(QtAV bash -x ./buildqtavforandroid.sh "${CMAKE_BUILD_TYPE}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" "${QT_DIR}" "${ANDROID_PREFIX}" "${ANDROID_PREFIX_FFMPEG}" "${ANDROID_TARGET}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMENT "Building QtAV.")
	elseif (UNIX)
		# Make sure that the pulseaudio library is available. Otherwise there won't be an audio backend for QtAV.
		find_library(PULSELIBRARY pulse PATHS /usr/lib64)

		if (NOT PULSELIBRARY)
			message(FATAL_ERROR "Missing pulse library.")
		endif ()

		find_library(OPENALLIBRARY openal PATHS /usr/lib64)

		if (NOT OPENALLIBRARY)
			message(FATAL_ERROR "Missing openal library.")
		endif ()

		set(CMAKE_INSTALL_RPATH "${QTAV_FFMPEG_LIB_DIR}" "${CMAKE_BINARY_DIR}/buildqtav/${QTAV_LIB_DIR}")
		set(CMAKE_SKIP_BUILD_RPATH FALSE)
		set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
		set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

		add_custom_target(QtAV bash -x ./buildqtavforlinux.sh "${CMAKE_BUILD_TYPE}" "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" COMMENT "Building QtAV.")
	else ()
		message(FATAL_ERROR "Other systems are not supported for the option USE_QTAV.")
	endif ()
endif ()

add_subdirectory(src)

add_library(gustavsfairylandlib ${fairyland_SRCS} ${fairyland_UI_HEADERS})
target_link_libraries(gustavsfairylandlib Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

if (USE_QTMEL)
	target_link_libraries(gustavsfairylandlib "${QT_MEL_LIBRARIES}" ${AV_LIBRARIES} ${OpenCV_LIBRARIES} ${ZLIB_LIBRARIES})
endif ()

if (USE_QTAV)
	add_dependencies(gustavsfairylandlib QtAV)
	target_link_libraries(gustavsfairylandlib Qt5::OpenGL ${QTAV_LIBRARIES})

	if (ANDROID)
		target_link_libraries(gustavsfairylandlib Qt5::AndroidExtras)
	endif ()
endif ()

set(RES_FILES "")
# Add a resource file containing the icon. This adds an icon to the executable.
if (WIN32)
	set(RES_FILES "${CMAKE_SOURCE_DIR}/win32/client.rc")
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif ()

if (ANDROID)
	add_library(gustavsfairyland SHARED src/main.cpp ${fairyland_RESOURCES_RCC} ${fairytale_QM_FILES} ${RES_FILES})
	set_property(TARGET gustavsfairyland PROPERTY VERSION "1")
	install(TARGETS gustavsfairyland LIBRARY DESTINATION lib)
else ()
	add_executable(gustavsfairyland src/main.cpp ${fairyland_RESOURCES_RCC} ${fairytale_QM_FILES} ${RES_FILES})
	install(TARGETS gustavsfairyland RUNTIME DESTINATION bin)
endif()

target_link_libraries(gustavsfairyland gustavsfairylandlib Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

if (USE_QTAV)
	target_link_libraries(gustavsfairyland Qt5::OpenGL ${QTAV_LIBRARIES})

	if (ANDROID)
		target_link_libraries(gustavsfairyland Qt5::AndroidExtras)
	endif ()

	include_directories("${CMAKE_BINARY_DIR}/qtav/src/")
	include_directories("${CMAKE_BINARY_DIR}/qtav/src/QtAV")
	include_directories("${CMAKE_BINARY_DIR}/qtav/widgets/")
	include_directories("${CMAKE_BINARY_DIR}/qtav/widgets/QtAVWidgets/")
endif ()

configure_file(doc/doxygen.conf doc/doxygen.conf)

if (USE_DOXYGEN)
	find_program(DOXYGEN_PATH doxygen)

	if (DOXYGEN_PATH)
		add_custom_target(Doxygen ALL ${DOXYGEN_PATH} "${CMAKE_BINARY_DIR}/doc/doxygen.conf" SOURCES ${fairyland_HEADERS} ${fairyland_SRCS} ${fairyland_UI})
	endif ()
endif ()

if (ANDROID)
	# This custom target is referenced by all install commands for Android.
	add_custom_target(CopyAssets ALL SOURCES ${fairytale_QM_FILES} ${CMAKE_SOURCE_DIR}/clips/gustav.xml ${CMAKE_SOURCE_DIR}/clips/custom.xml)

	foreach (f ${fairytale_QM_FILES})
		get_filename_component(fileName "${f}" NAME)
		add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy "${f}" ${CMAKE_BINARY_DIR}/assets/translations/${fileName} )
	endforeach ()
else ()
	install(FILES ${fairytale_QM_FILES} DESTINATION share/gustavsfairyland/translations)
endif ()

if (UNIX)
	find_program(DESKTOP_FILE_VALIDATE_PATH desktop-file-validate)

	if (DESKTOP_FILE_VALIDATE_PATH)
		message(STATUS "Validating .desktop files with program ${DESKTOP_FILE_VALIDATE_PATH}.")
		add_custom_target(ValidateDesktopFile ALL COMMAND ${DESKTOP_FILE_VALIDATE_PATH} ${CMAKE_SOURCE_DIR}/gustavsfairyland.desktop SOURCES gustavsfairyland.desktop)
	endif ()

	install(FILES gustavsfairyland.desktop DESTINATION share/applications)
	install(FILES gustavsfairyland.png DESTINATION share/icons/hicolor/48x48/apps)
endif ()

if (USE_COMPRESSED_FILES)
	set(COMPRESSED_SUFFIX "unix")

	if (WIN32)
		set(COMPRESSED_SUFFIX "windows")
	elseif (ANDROID)
		set(COMPRESSED_SUFFIX "android")
	endif ()

	set(COMPRESSED_VIDEO_FILES_EXPRESSION "${CMAKE_SOURCE_DIR}/clips/*/*_compressed_${COMPRESSED_SUFFIX}*")
	message(STATUS "Finding compressed video files: ${COMPRESSED_VIDEO_FILES_EXPRESSION}")
	file(GLOB_RECURSE COMPRESSED_FILES_VIDEOS "${COMPRESSED_VIDEO_FILES_EXPRESSION}")
	file(GLOB_RECURSE COMPRESSED_FILES_SOUNDS "${CMAKE_SOURCE_DIR}/clips/*/*_compressed.wav")
	file(GLOB_RECURSE COMPRESSED_FILES_MUSIC "${CMAKE_SOURCE_DIR}/clips/*/*_compressed.mp3")
	list(APPEND COMPRESSED_FILES ${COMPRESSED_FILES_VIDEOS} ${COMPRESSED_FILES_SOUNDS} ${COMPRESSED_FILES_MUSIC})
else ()
	file(GLOB_RECURSE COMPRESSED_FILES_AVI "${CMAKE_SOURCE_DIR}/clips/*/*.avi")
	file(GLOB_RECURSE COMPRESSED_FILES_MKV "${CMAKE_SOURCE_DIR}/clips/*/*.mkv")
	file(GLOB_RECURSE COMPRESSED_SPEECH_FILES "${CMAKE_SOURCE_DIR}/clips/*/*.wav")
	file(GLOB_RECURSE COMPRESSED_MUSIC_FILES "${CMAKE_SOURCE_DIR}/clips/*/*.mp3")

	# TODO remove all files with a _compressed expression!
	list(APPEND COMPRESSED_FILES ${COMPRESSED_VIDEO_FILES_AVI} ${COMPRESSED_VIDEO_FILES_MKV} ${COMPRESSED_SPEECH_FILES} ${COMPRESSED_MUSIC_FILES})
endif ()

# the default fairyland package gustav
# Install all file paths manually and only use the specified files. This allows you to use compressed video files for example.
if (ANDROID)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/clips/gustav.xml ${CMAKE_BINARY_DIR}/assets/clips/gustav.xml)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/clips/custom.xml ${CMAKE_BINARY_DIR}/assets/clips/custom.xml)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/logo.jpg ${CMAKE_BINARY_DIR}/res/drawable/logo.jpg)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/logo.jpg ${CMAKE_BINARY_DIR}/bin/res/drawable/logo.jpg)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/translations/fairylandstrings.xml ${CMAKE_BINARY_DIR}/res/values/fairylandstrings.xml)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/translations/fairylandstrings.xml ${CMAKE_BINARY_DIR}/bin/res/values/fairylandstrings.xml)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/translations/fairylandstrings.xml ${CMAKE_BINARY_DIR}/res/values-de/fairylandstrings-de.xml)
	add_custom_command(TARGET CopyAssets COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/translations/fairylandstrings.xml ${CMAKE_BINARY_DIR}/bin/res/values-de/fairylandstrings-de.xml)
else ()
	install(FILES clips/gustav.xml clips/custom.xml DESTINATION share/gustavsfairyland/clips)
endif ()
message(STATUS "Compressed files: ${COMPRESSED_FILES}")
file(GLOB_RECURSE IMAGE_FILES_JPG "${CMAKE_SOURCE_DIR}/clips/*/*.jpg")
file(GLOB_RECURSE IMAGE_FILES_PNG "${CMAKE_SOURCE_DIR}/clips/*/*.png")
list(APPEND IMAGE_FILES ${IMAGE_FILES_JPG} ${IMAGE_FILES_PNG})
message(STATUS "Image files: ${IMAGE_FILES}")
list(APPEND INSTALL_FILES ${COMPRESSED_FILES} ${IMAGE_FILES})

# Specify the install directory for every found file.
foreach (f ${INSTALL_FILES})
	#message(STATUS "Install: ${f}")
	get_filename_component(f_name "${f}" NAME)
	string(REPLACE "_compressed" "" fileName "${f_name}")
	string(REPLACE "_windows" "" fileName "${fileName}")
	string(REPLACE "_unix" "" fileName "${fileName}")
	string(REPLACE "_android" "" fileName "${fileName}")
	get_filename_component(f_dir "${f}" DIRECTORY)
	string(FIND "${f_dir}" "clips/" f_clipsPos REVERSE)
	string(LENGTH "clips/" clipsLength)
	string(LENGTH "${f_dir}" dirLength)
	math(EXPR subStringPos "${f_clipsPos}+${clipsLength}")
	math(EXPR subStringLength "${dirLength}-${subStringPos}")
	string(SUBSTRING "${f_dir}" ${subStringPos} ${subStringLength} dirName)
	set(INSTALL_DIR "${dirName}")
	message(STATUS "Install path for \"${f}\": ${INSTALL_DIR}/${fileName}")
	install(FILES "${f}" DESTINATION share/gustavsfairyland/clips/${INSTALL_DIR} RENAME "${fileName}")

	# Copy files into assets folder for Android.
	if (ANDROID)
		add_custom_command(TARGET CopyAssets PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${f}" ${CMAKE_BINARY_DIR}/assets/clips/${INSTALL_DIR}/${fileName})
	endif ()
endforeach ()

# Installs all required Qt binary files on Windows.
# The program windeployqt generates all files into the binary directory which have to be installed
if (WIN32)
	# Make sure the output directory is cleaned everytime a clean is done since there might be different DLLs when distributing the relase version.
	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/qtdeployment")
	find_program(WINDEPLOYQT_PROGRAM windeployqt)

	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set(WINDEPLOYQT_ARGS "--debug")
	else ()
		set(WINDEPLOYQT_ARGS "--release")
	endif ()
	add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:gustavsfairyland> ${WINDEPLOYQT_ARGS} --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS gustavsfairyland)
	install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION bin)
endif ()

if (ANDROID)
	if (USE_ANDROID_JAVA_PLAYER)
		set(JAVA_HOME /usr/java/default)
		set(Java_JAVA_EXECUTABLE "${JAVA_HOME}/bin/java")
		set(Java_JAVAC_EXECUTABLE "${JAVA_HOME}/bin/javac")
		set(Java_JAVADOC_EXECUTABLE "${JAVA_HOME}/bin/javadoc")
		set(Java_JAR_EXECUTABLE "${JAVA_HOME}/bin/jar")
		set(Java_JAVAH_EXECUTABLE "${JAVA_HOME}/bin/javah")
		find_package(Java REQUIRED)
		include(UseJava)

		set(CMAKE_JAVA_COMPILE_FLAGS -source 7 -target 7)
		set(CMAKE_JAVA_INCLUDE_PATH "${QT_ANDROID_SDK_ROOT}/platforms/android-${ANDROID_API_LEVEL}/android.jar")
		add_jar(PlayerJar SOURCES src/android/Player.java OUTPUT_NAME Player INCLUDE_JARS "${QT_ANDROID_SDK_ROOT}/platforms/android-${ANDROID_API_LEVEL}/android.jar" OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/libs")
	endif ()

	# Copy all clips and xml files into a subfolder in assets
	#add_custom_target(CopyAssets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/clips ${CMAKE_BINARY_DIR}/assets/clips)
	# Use the following command to generate a signing key
	# keytool -genkey -v -keystore my-release-key.keystore -alias alias_name -keyalg RSA -keysize 2048 -validity 10000
	# NOTE add PACKAGE_SOURCES "${CMAKE_SOURCE_DIR}/clips" to install all clips
	# PACKAGE_NAME "org.tacaproductions.gustavsfairyland"
	add_qt_android_apk(my_app_apk gustavsfairyland NAME "@string/ApplicationName"
		DEPENDS
		${QTAV_LIBRARIES}
	)
	add_dependencies(my_app_apk CopyAssets QtAV gustavsfairyland)

	if (USE_ANDROID_JAVA_PLAYER)
		add_dependencies(my_app_apk PlayerJar)
	endif ()
endif ()

set(CPACK_PACKAGE_NAME "gustavsfairyland")
set(CPACK_PACKAGE_VENDOR "https://gustavsfairyland.wordpress.com")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A game to create custom fairytales narrated by Gustav Mahler himself.")
set(CPACK_PACKAGE_VERSION "${gustavsfairyland_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${gustavsfairyland_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${gustavsfairyland_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${gustavsfairyland_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gustavsfairyland")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_EXECUTABLES "gustavsfairyland" "Gustav's Fairyland")
set(CPACK_CREATE_DESKTOP_LINKS "gustavsfairyland")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources\\\\logo.jpg")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")

# Windows NSIS
set(CPACK_NSIS_URL_INFO_ABOUT "https://gustavsfairyland.wordpress.com")
# improve the compression since we have many clips and Qt binaries
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
string(REPLACE "/" "\\\\" WINDOWS_EXE_PATH "$<TARGET_FILE:gustavsfairyland>")
set(CPACK_NSIS_INSTALLED_ICON_NAME "${WINDOWS_EXE_PATH}")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/resources/logo.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/resources/logo.ico")
# The path page is confusing for normal users and it is not necessary for the game.
set(CPACK_NSIS_MODIFY_PATH OFF)

# Fedora RPM
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_GROUP "Amusements/Games")
set(CPACK_RPM_PACKAGE_URL "https://gustavsfairyland.wordpress.com")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtsvg >= 5.5.1, qt5-qttranslations >= 5.5.1")

# Debian and Ubuntu DEB
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5, libqt5widgets5, libqt5multimedia5, libqt5multimediawidgets5, libqt5xml5, libqt5svg5, qttranslations5-l10n") # Ubuntu dependency
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://gustavsfairyland.wordpress.com")

include(CPack)
