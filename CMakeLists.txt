cmake_minimum_required(VERSION 2.8.11)
project(fairyland)

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
)

qt5_wrap_ui(fairyland_UI_HEADERS ${fairyland_UI})

QT5_ADD_RESOURCES(fairyland_RESOURCES_RCC resource.qrc)

add_library(fairylandlib ${fairyland_SRCS} ${fairyland_UI_HEADERS})
target_link_libraries(fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

add_executable(fairyland main.cpp ${fairyland_RESOURCES_RCC})
target_link_libraries(fairyland fairylandlib Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Xml Qt5::Svg)

install(TARGETS fairyland RUNTIME DESTINATION bin)

install(FILES fairyland.desktop DESTINATION share/applications)

# the default fairyland
install(FILES clips/gustav.xml DESTINATION clips)
install(FILES clips/boar/boar.jpg DESTINATION clips/boar/)
install(FILES clips/boar/boar.mkv DESTINATION clips/boar/)
install(FILES clips/boar/boar.wav DESTINATION clips/boar/)
install(FILES clips/castle/castle.jpg DESTINATION clips/castle/)
install(FILES clips/castle/castle.mkv DESTINATION clips/castle/)
install(FILES clips/castle/castle.wav DESTINATION clips/castle/)
install(FILES clips/cave/cave.jpg DESTINATION clips/cave/)
install(FILES clips/cave/cave.mkv DESTINATION clips/cave/)
install(FILES clips/cave/cave.wav DESTINATION clips/cave/)
install(FILES clips/caveman/caveman.jpg DESTINATION clips/caveman/)
install(FILES clips/caveman/caveman.mkv DESTINATION clips/caveman/)
install(FILES clips/caveman/caveman.wav DESTINATION clips/caveman/)
install(FILES clips/caveman/chess.jpg DESTINATION clips/chess/)
install(FILES clips/caveman/chess.mkv DESTINATION clips/chess/)
install(FILES clips/caveman/chess.wav DESTINATION clips/chess/)
install(FILES clips/caveman/climbing.jpg DESTINATION clips/climbing/)
install(FILES clips/caveman/climbing.mkv DESTINATION clips/climbing/)
install(FILES clips/caveman/climbing.wav DESTINATION clips/climbing/)
install(FILES clips/crazy/runterholer.jpg DESTINATION clips/crazy/)
install(FILES clips/crazy/runterholer.mkv DESTINATION clips/crazy/)
install(FILES clips/crazy/crazy.wav DESTINATION clips/crazy/)
install(FILES clips/opajott/opajott1.mkv DESTINATION clips/opajott/)
install(FILES clips/opajott/opajott1.jpg DESTINATION clips/opajott/)
install(FILES clips/opajott/opajott.wav DESTINATION clips/opajott/)
install(FILES clips/steampunk/steampunk.jpg DESTINATION clips/steampunk/)
install(FILES clips/steampunk/steampunk.avi DESTINATION clips/steampunk/)
install(FILES clips/steampunk/steampunk.wav DESTINATION clips/steampunk/)
install(FILES clips/vampir/vampir.png DESTINATION clips/vampir/)
install(FILES clips/vampir/vampir.avi DESTINATION clips/vampir/)
install(FILES clips/vampir/vampir.wav DESTINATION clips/vampir/)
install(FILES clips/werwolf/werwolf.png DESTINATION clips/werwolf/)
install(FILES clips/werwolf/werwolf.avi DESTINATION clips/werwolf/)
install(FILES clips/werwolf/werwolf.wav DESTINATION clips/werwolf/)
install(FILES clips/mahler/mahler.jpg DESTINATION clips/mahler/)
install(FILES clips/mahler/mahler.avi DESTINATION clips/mahler/)
install(FILES clips/mahler/mahler.wav DESTINATION clips/mahler/)
install(FILES clips/gate/gate.jpg DESTINATION clips/gate/)
install(FILES clips/gate/gate.mkv DESTINATION clips/gate/)
install(FILES clips/gate/gate.wav DESTINATION clips/gate/)
install(FILES clips/maninthebox/maninthebox.jpg DESTINATION clips/maninthebox/)
install(FILES clips/maninthebox/maninthebox.mkv DESTINATION clips/maninthebox/)
install(FILES clips/maninthebox/maninthebox.wav DESTINATION clips/maninthebox/)
install(FILES clips/prison/prison.jpg DESTINATION clips/prison/)
install(FILES clips/prison/prison.mkv DESTINATION clips/prison/)
install(FILES clips/prison/prison.wav DESTINATION clips/prison/)
install(FILES clips/robery/robery.jpg DESTINATION clips/robery/)
install(FILES clips/robery/robery.mkv DESTINATION clips/robery/)
install(FILES clips/robery/robery.wav DESTINATION clips/robery/)

install(FILES clips/throne/throne.jpg DESTINATION clips/throne/)
install(FILES clips/throne/throne.mkv DESTINATION clips/throne/)
install(FILES clips/throne/throne.wav DESTINATION clips/throne/)

install(FILES clips/tower/tower.jpg DESTINATION clips/tower/)
install(FILES clips/tower/tower.mkv DESTINATION clips/tower/)
install(FILES clips/tower/tower.wav DESTINATION clips/tower/)

install(FILES clips/train/train.jpg DESTINATION clips/train/)
install(FILES clips/train/train.mkv DESTINATION clips/train/)
install(FILES clips/train/train.wav DESTINATION clips/train/)

install(FILES clips/trash/trash.jpg DESTINATION clips/trash/)
install(FILES clips/trash/trash.mkv DESTINATION clips/trash/)
install(FILES clips/trash/trash.wav DESTINATION clips/trash/)

install(FILES clips/treasure/treasure.jpg DESTINATION clips/treasure/)
install(FILES clips/treasure/treasure.mkv DESTINATION clips/treasure/)
install(FILES clips/treasure/treasure.wav DESTINATION clips/treasure/)

install(FILES clips/well/well.jpg DESTINATION clips/well/)
install(FILES clips/well/well.mkv DESTINATION clips/well/)
install(FILES clips/well/well.wav DESTINATION clips/well/)

# Installs all required Qt binary files on Windows.
# The program windeployqt generates all files into the binary directory which have to be installed
if (WIN32)
    # Make sure the output directory is cleaned everytime a clean is done since there might be different DLLs when distributing the relase version.
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_BINARY_DIR}/qtdeployment")
    find_program(WINDEPLOYQT_PROGRAM windeployqt)
    add_custom_target(deployqt ALL ${WINDEPLOYQT_PROGRAM} $<TARGET_FILE:fairyland> --dir "${CMAKE_BINARY_DIR}/qtdeployment" DEPENDS fairyland)
    install(DIRECTORY "${CMAKE_BINARY_DIR}/qtdeployment/" DESTINATION bin)
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

# Fedora package
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_GROUP "games")
set(CPACK_RPM_PACKAGE_URL "http://wc3lib.org")
set(CPACK_RPM_PACKAGE_REQUIRES "qt5-qtbase-gui >= 5.5.1, qt5-qtmultimedia >= 5.5.1, qt5-qtxmlpatterns >= 5.5.1")

# Debian
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamino Dauth")
set(CPACK_PACKAGE_CONTACT "tamino@cdauth.eu")

include(CPack)
