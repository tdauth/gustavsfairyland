cmake_minimum_required(VERSION 2.8.11)
project(fairytale)

# install libs
include(InstallRequiredSystemLibraries)

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

set(fairytale_SRCS
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
)

set(fairytale_UI
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
)

qt5_wrap_ui(fairytale_UI_HEADERS ${fairytale_UI})

QT5_ADD_RESOURCES(fairytale_RESOURCES_RCC resource.qrc)

add_library(fairytalelib ${fairytale_SRCS} ${fairytale_UI_HEADERS})
target_link_libraries(fairytalelib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml)

add_executable(fairytale main.cpp ${fairytale_RESOURCES_RCC})
target_link_libraries(fairytale fairytalelib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml)

install(TARGETS fairytale RUNTIME DESTINATION bin)

install(FILES fairytale.desktop DESTINATION share/applications)

# the default fairytale
install(FILES clips/gustav.xml DESTINATION clips)
install(FILES clips/crazy/runterholer.jpg DESTINATION clips/crazy/)
install(FILES clips/crazy/runterholer.mkv DESTINATION clips/crazy/)
install(FILES clips/opajott/opajott1.mkv DESTINATION clips/opajott/)
install(FILES clips/opajott/opajott1.jpg DESTINATION clips/opajott/)
install(FILES clips/steampunk/steampunk.jpg DESTINATION clips/steampunk/)
install(FILES clips/vampir/vampir.png DESTINATION clips/vampir/)
install(FILES clips/werwolf/werwolf.png DESTINATION clips/werwolf/)
install(FILES clips/mahler/mahler.jpg DESTINATION clips/mahler/)
install(FILES clips/mahler/mahler.avi DESTINATION clips/mahler/)
install(FILES clips/siege.jpg DESTINATION clips)
install(FILES clips/marry.jpg DESTINATION clips)
install(FILES clips/torture.png DESTINATION clips)
install(FILES clips/burn.jpg DESTINATION clips)

# Installs all required Qt binary files on Windows.
# The program windeployqt generates all files into the binary directory which have to be installed
if (WIN32)
    find_program(WINDEPLOYQT_PROGRAM windeployqt)
    add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:fairytale> --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS fairytale)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION bin)
endif ()

add_subdirectory(test)

set(CPACK_PACKAGE_NAME "gustavsfairyland")
set(CPACK_PACKAGE_VENDOR "wc3lib.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A game to create custom fairytales narrated by Gustav Mahler himself.")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gustavsfairyland")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

# NSIS Windows
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_CREATE_DESKTOP_LINKS "fairytale")

# Fedora package
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_GROUP "games")
set(CPACK_RPM_PACKAGE_URL "http://wc3lib.org")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtxmlpatterns >= 5.5.1")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")

include(CPack)
