cmake_minimum_required(VERSION 2.8.11)
project(fairyland)

if (ANDROID)
	include("cmake/AddQtAndroidApk.cmake" REQUIRED)
	include_directories("/home/tamino/android-ndk-r12b/sources/android/native_app_glue/")
endif ()

enable_testing()

add_compile_options(-std=c++11 -Wall)

message(STATUS "Compiling for ${CMAKE_SYSTEM_NAME}")

option(USE_COMPRESSED_VIDEOS "Enable this option to install much smaller videos." FALSE)

# Validate XML files if possible
find_program(XMLLINT_PATH xmllint)

if (XMLLINT_PATH)
	message(STATUS "Validating XML files.")
	set(XML_FILES
		"${CMAKE_SOURCE_DIR}/clips/gustav.xml"
		"${CMAKE_SOURCE_DIR}/test/clips.xml"
	)
	# TODO check against an XML schema as well
	add_custom_target(ValidateXMLFiles ALL ${XMLLINT_PATH} --noout "${CMAKE_SOURCE_DIR}/clips/gustav.xml" "${CMAKE_SOURCE_DIR}/test/clips.xml"  SOURCES ${XML_FILES})
endif ()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Handle .rcc files automatically
set(CMAKE_AUTORCC ON)

# Find all Qt modules which are required
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Multimedia)
find_package(Qt5Xml)
find_package(Qt5MultimediaWidgets)
find_package(Qt5Xml)
find_package(Qt5Svg)
find_package(Qt5LinguistTools)

set(fairyland_SRCS
	aboutdialog.cpp
	fairytale.cpp
	floatingclip.cpp
	clip.cpp
	door.cpp
	player.cpp
	iconbutton.cpp
	clipsdialog.cpp
	clippackage.cpp
	clippackagedialog.cpp
	clippackageeditor.cpp
	clipeditor.cpp
	customfairytaledialog.cpp
	gamemode.cpp
	gamemodedialog.cpp
	gamemodemoving.cpp
	gamemodeoneoutoffour.cpp
	iconbutton.cpp
	roomwidget.cpp
	settingsdialog.cpp
	wondialog.cpp
	iconlabel.cpp
	highscores.cpp
	bonusclip.cpp
)

set(fairyland_UI
	ui/aboutdialog.ui
	ui/mainwindow.ui
	ui/player.ui
	ui/clips.ui
	ui/clippackagedialog.ui
	ui/clippackageeditor.ui
	ui/clipeditor.ui
	ui/customfairytale.ui
	ui/gamemodedialog.ui
	ui/settings.ui
	ui/wondialog.ui
	ui/highscores.ui
)

qt5_wrap_ui(fairyland_UI_HEADERS ${fairyland_UI})

set(fairyland_TRANSLATIONS
	translations/en.ts
	translations/de.ts
)

file(GLOB_RECURSE fairytale_TRANSLATION_SRCS GLOB_RECURSE "${CMAKE_BINARY_DIR}" "*.cpp" "*.h")
QT5_CREATE_TRANSLATION(fairytale_QM_FILES ${fairytale_TRANSLATION_SRCS} ${fairyland_TRANSLATIONS})

QT5_ADD_RESOURCES(fairyland_RESOURCES_RCC resource.qrc)

add_library(fairylandlib ${fairyland_SRCS} ${fairyland_UI_HEADERS})
target_link_libraries(fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

if (ANDROID)
	add_library(fairyland SHARED main.cpp ${fairyland_RESOURCES_RCC} ${fairytale_QM_FILES})
	set_property(TARGET fairyland PROPERTY VERSION "1")
	install(TARGETS fairyland LIBRARY DESTINATION lib)
else ()
	add_executable(fairyland main.cpp ${fairyland_RESOURCES_RCC} ${fairytale_QM_FILES})
	install(TARGETS fairyland RUNTIME DESTINATION bin)
endif()

target_link_libraries(fairyland fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

configure_file(doc/doxygen.conf doc/doxygen.conf)

install(FILES ${fairytale_QM_FILES} DESTINATION share/gustavsfairyland/translations)
install(FILES fairyland.desktop DESTINATION share/applications)

if (USE_COMPRESSED_VIDEOS)
	file(GLOB_RECURSE COMPRESSED_VIDEO_FILES "${CMAKE_SOURCE_DIR}/clips/*/*_compressed*")
else ()
	file(GLOB_RECURSE COMPRESSED_VIDEO_FILES_AVI "${CMAKE_SOURCE_DIR}/clips/*/*.avi")
	file(GLOB_RECURSE COMPRESSED_VIDEO_FILES_MKV "${CMAKE_SOURCE_DIR}/clips/*/*.mkv")
	list(APPEND COMPRESSED_VIDEO_FILES ${COMPRESSED_VIDEO_FILES_AVI} ${COMPRESSED_VIDEO_FILES_MKV})
endif ()

# the default fairyland package gustav
# Install all file paths manually and only use the specified files. This allows you to use compressed video files for example.
install(FILES clips/gustav.xml DESTINATION share/gustavsfairyland/clips)
message(STATUS "Compressed files: ${COMPRESSED_VIDEO_FILES}")
file(GLOB_RECURSE IMAGE_FILES_JPG "${CMAKE_SOURCE_DIR}/clips/*/*.jpg")
file(GLOB_RECURSE IMAGE_FILES_PNG "${CMAKE_SOURCE_DIR}/clips/*/*.png")
list(APPEND IMAGE_FILES ${IMAGE_FILES_JPG} ${IMAGE_FILES_PNG})
message(STATUS "Image files: ${IMAGE_FILES}")
file(GLOB_RECURSE SPEECH_FILES "${CMAKE_SOURCE_DIR}/clips/*/*.wav")
message(STATUS "Sound files: ${SPEECH_FILES}")
list(APPEND INSTALL_FILES ${COMPRESSED_VIDEO_FILES} ${IMAGE_FILES} ${SPEECH_FILES})

# This custom target is referenced by all install commands for Android.
if (ANDROID)
	add_custom_target(CopyAssets ALL)
endif ()

# Specify the install directory for every found file.
foreach (f ${INSTALL_FILES})
	#message(STATUS "Install: ${f}")
	get_filename_component(f_name "${f}" NAME)
	string(REPLACE "_compressed" "" fileName "${f_name}")
	get_filename_component(f_dir "${f}" DIRECTORY)
	string(FIND "${f_dir}" "clips/" f_clipsPos REVERSE)
	string(LENGTH "clips/" clipsLength)
	string(LENGTH "${f_dir}" dirLength)
	math(EXPR subStringPos "${f_clipsPos}+${clipsLength}")
	math(EXPR subStringLength "${dirLength}-${subStringPos}")
	string(SUBSTRING "${f_dir}" ${subStringPos} ${subStringLength} dirName)
	set(INSTALL_DIR "${dirName}")
	message(STATUS "Install path: ${INSTALL_DIR}/${fileName}")
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
    add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:fairyland> --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS fairyland)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION bin)
endif ()

if (ANDROID)
	# Copy all clips and xml files into a subfolder in assets
	#add_custom_target(CopyAssets ALL COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/clips ${CMAKE_BINARY_DIR}/assets/clips)
	# NOTE add PACKAGE_SOURCES "${CMAKE_SOURCE_DIR}/clips" to install all clips
	add_qt_android_apk(my_app_apk fairyland)
endif ()

add_subdirectory(test)

set(CPACK_PACKAGE_NAME "gustavsfairyland")
set(CPACK_PACKAGE_VENDOR "wc3lib.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A game to create custom fairylands narrated by Gustav Mahler himself.")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/clips/mahler\\\\mahler.jpg")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gustavsfairyland")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_EXECUTABLES "fairyland" "Gustav's Fairland")
set(CPACK_CREATE_DESKTOP_LINKS "fairyland")

# NSIS Windows
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_URL_INFO_ABOUT "http://wc3lib.org")
# improve the compression since we have many clips and Qt binaries
set(CPACK_NSIS_COMPRESSOR "/SOLID lzma")

# Fedora package
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_GROUP "games")
set(CPACK_RPM_PACKAGE_URL "http://wc3lib.org")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtxmlpatterns >= 5.5.1")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")

include(CPack)