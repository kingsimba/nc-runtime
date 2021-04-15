cmake_minimum_required(VERSION 3.16)

SET(PROJ nc-runtime)

project(${PROJ} VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (WIN32)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /GF")	# enable string pool
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Ob1")	# inline only __inline
else()
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
	add_compile_options(-Werror)  # make all warning into error
	add_compile_options(-Wno-unused-variable)
	add_compile_options(-Wno-unused-parameter)
endif()

########################################################################################
# nc-runtime.lib

file(GLOB_RECURSE SRC "src/*.c" "src/*.cpp" "src/*.h" "include/*.h")
file(GLOB_RECURSE JANSSON "3rd-party/src/jansson/*.cpp" "3rd-party/include/jansson/*.h")

add_library(${PROJ} STATIC ${SRC} ${JANSSON})
set_target_properties(${PROJ} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${PROJ} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${PROJ} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_include_directories(${PROJ} PRIVATE "src" "include/nc-runtime" "3rd-party/include")
add_definitions(-D_CRTDBG_MAP_ALLOC)

target_precompile_headers(${PROJ} PRIVATE "src/stdafx_nc_runtime.h")

########################################################################################
# nc-runtime-test

set(TEST_PROJ "nc-runtime-test")
file(GLOB_RECURSE TEST_SRC "test/*.cpp" "test/*.h" "3rd-party/src/googletest-1.10.0/*.h" "3rd-party/src/googletest-1.10.0/src/gtest-all.cc")
add_executable(${TEST_PROJ} ${TEST_SRC})
target_link_libraries(${TEST_PROJ} PRIVATE "nc-runtime")

if (NOT WIN32)
	target_link_libraries(${TEST_PROJ} PRIVATE pthread)	# google test needs pthread on Linux
endif()

set_target_properties(${TEST_PROJ} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${TEST_PROJ} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set_target_properties(${TEST_PROJ} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_include_directories(${TEST_PROJ} PRIVATE "src" "include/nc-runtime" "3rd-party/include" "3rd-party/src/googletest-1.10.0")
target_precompile_headers(${TEST_PROJ} PRIVATE "test/stdafx_nc_runtime_test.h")
