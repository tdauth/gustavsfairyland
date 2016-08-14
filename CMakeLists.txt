cmake_minimum_required(VERSION 2.8.11)
project(fairyland)

# install libs
include(InstallRequiredSystemLibraries)
#include("cmake/Apk.cmake" REQUIRED)
if (ANDROID)
	include("cmake/AddQtAndroidApk.cmake" REQUIRED)
	include_directories("/home/tamino/android-ndk-r12b/sources/android/native_app_glue/")
endif ()

enable_testing()

add_compile_options(-std=c++11 -Wall)

message(STATUS "Compiling for ${CMAKE_SYSTEM_NAME}")

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

if (ANDROID)
    set(fairyland_SRCS ${fairyland_SRCS} mainandroid.cpp)
endif ()

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

QT5_ADD_RESOURCES(fairyland_RESOURCES_RCC resource.qrc)

add_library(fairylandlib ${fairyland_SRCS} ${fairyland_UI_HEADERS})
target_link_libraries(fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

if (ANDROID)
	add_library(fairyland SHARED main.cpp ${fairyland_RESOURCES_RCC})
	set_property(TARGET fairyland PROPERTY VERSION "1")
	install(TARGETS fairyland LIBRARY DESTINATION lib)
else ()
	add_executable(fairyland main.cpp ${fairyland_RESOURCES_RCC})
	install(TARGETS fairyland RUNTIME DESTINATION bin)
endif()

target_link_libraries(fairyland fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

configure_file(doc/doxygen.conf doc/doxygen.conf)

install(FILES fairyland.desktop DESTINATION share/applications)

# the default fairyland
install(DIRECTORY clips DESTINATION clips)

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
