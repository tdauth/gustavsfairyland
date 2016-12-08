# Only create Unit Tests in debug mode. Otherwise the test lib is linked and might cause creating a console on Windows.
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	message(STATUS "Adding test directory.")
	add_subdirectory(test)
endif ()