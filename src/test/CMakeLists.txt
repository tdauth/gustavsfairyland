find_package(Qt5Test REQUIRED)

add_executable(cliptest cliptest.cpp)
target_link_libraries(cliptest Qt5::Core Qt5::Test gustavsfairylandlib)
add_test(NAME ClipTest COMMAND ./cliptest)
configure_file(clips.xml clips.xml COPYONLY)

add_executable(recordtest recordtest.cpp)
target_link_libraries(recordtest Qt5::Core Qt5::Test gustavsfairylandlib)
add_test(NAME RecordTest COMMAND ./recordtest)

include_directories("${CMAKE_BINARY_DIR}")