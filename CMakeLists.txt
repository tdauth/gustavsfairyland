cmake_minimum_required(VERSION 2.8.11)
project(fairytale)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Multimedia)
find_package(Qt5MultimediaWidgets)
find_package(Qt5Xml)

set(fairytale_SRCS fairytale.cpp main.cpp clip.cpp player.cpp)

qt5_wrap_ui(fairytale_UI_HEADERS mainwindow.ui player.ui)

add_executable(fairytale ${fairytale_SRCS} ${fairytale_UI_HEADERS})
target_link_libraries(fairytale Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml)

install(TARGETS fairytale RUNTIME DESTINATION bin)

#qt5_add_translation( qm_files ts_files ... )

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "fairytale")
set(CPACK_PACKAGE_VENDOR "CMake.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MyLib - CPack Component Installation Example")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CPack Component Example")
SET(CPACK_NSIS_MODIFY_PATH ON)

INCLUDE(CPack)