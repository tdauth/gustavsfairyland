find_package(Qt5Test REQUIRED)

add_executable(cliptest cliptest.cpp)
target_link_libraries(cliptest Qt5::Core Qt5::Test fairylandlib)
add_test(NAME ClipTest COMMAND ./cliptest)
configure_file(clips.xml clips.xml COPYONLY)