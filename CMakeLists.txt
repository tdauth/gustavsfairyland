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


# Installs all required Qt binary files on Windows. This should be a temporary solution. It would be much better if a CMake module would do this.
if (WIN32)
    set(qt_binary_files
        qt/Qt5Core.dll
        qt/Qt5Cored.dll
        qt/Qt5Gui.dll
        qt/Qt5Guid.dll
        qt/Qt5Multimedia.dll
        qt/Qt5Multimediad.dll
        qt/Qt5Networkd.dll
        qt/Qt5Network.dll
        qt/Qt5MultimediaWidgetsd.dll
        qt/Qt5MultimediaWidgets.dll
        qt/Qt5OpenGLd.dll
        qt/Qt5OpenGL.dll
        qt/Qt5Widgetsd.dll
        qt/Qt5Widgets.dll
        qt/Qt5Xml.dll
        qt/Qt5Xmld.dll
    )
    install(FILES ${qt_binary_files} DESTINATION bin)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION bin COMPONENT Libraries)
endif ()

add_subdirectory(test)

#qt5_add_translation( qm_files ts_files ... )

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
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtxmlpatterns >= 5.5.1")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")

include(CPack)
