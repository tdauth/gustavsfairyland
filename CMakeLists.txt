cmake_minimum_required(VERSION 2.8.11)
project(fairytale)

enable_testing()

add_compile_options(-std=c++11 -Wall)

message(STATUS "Compiling for ${CMAKE_SYSTEM_NAME}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Multimedia)
if (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"))
	find_package(Qt5MultimediaWidgets)
	find_package(Qt5Xml)
endif ()

add_subdirectory(ui)

set(fairytale_SRCS
	fairytale.cpp
	clip.cpp
	player.cpp
	iconbutton.cpp
	clipsdialog.cpp
	clippackage.cpp
	clippackagedialog.cpp
	clippackageeditor.cpp
	clipeditor.cpp
	customfairytaledialog.cpp
	gamemode.cpp
	gamemodemoving.cpp
	gamemodeoneoutoffour.cpp
	iconbutton.cpp
)

set(fairytale_UI
	ui/mainwindow.ui
	ui/player.ui
	ui/clips.ui
	ui/clippackagedialog.ui
	ui/clippackageeditor.ui
	ui/clipeditor.ui
	ui/customfairytale.ui
)

qt5_wrap_ui(fairytale_UI_HEADERS ${fairytale_UI})

add_library(fairytalelib ${fairytale_SRCS} ${fairytale_UI_HEADERS})
target_link_libraries(fairytalelib Qt5::Core Qt5::Widgets Qt5::Multimedia)
if (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows"))
	target_link_libraries(fairytalelib Qt5::MultimediaWidgets Qt5::Xml)
endif ()

add_executable(fairytale main.cpp)
target_link_libraries(fairytale fairytalelib)

install(TARGETS fairytale RUNTIME DESTINATION bin)

install(FILES fairytale.desktop DESTINATION share/applications)

add_subdirectory(test)

#qt5_add_translation( qm_files ts_files ... )

set(CPACK_PACKAGE_NAME "fairytale")
set(CPACK_PACKAGE_VENDOR "wc3lib.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A game to create custom fairytales.")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPack Component Example")
set(CPACK_NSIS_MODIFY_PATH ON)

# Fedora package
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_GROUP "games")
set(CPACK_RPM_PACKAGE_URL "http://wc3lib.org")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtxmlpatterns >= 5.5.1")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")


include(CPack)