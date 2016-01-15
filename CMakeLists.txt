cmake_minimum_required(VERSION 2.6)
project(fairytale)
find_package(Qt4 REQUIRED COMPONENTS QtGui QtCore Phonon)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(fairytale_SRCS fairytale.cpp main.cpp clip.cpp)

QT4_WRAP_UI(fairytale_UI_HEADERS mainwindow.ui)

qt4_automoc(${fairytale_SRCS} ${fairytale_UI_HEADERS})
add_executable(fairytale ${fairytale_SRCS} ${fairytale_UI_HEADERS})
target_link_libraries(fairytale ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_PHONON_LIBRARY})

install(TARGETS fairytale RUNTIME DESTINATION bin)