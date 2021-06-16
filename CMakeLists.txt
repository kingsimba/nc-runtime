cmake_minimum_required(VERSION 3.16)
project(nc_runtime C CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
find_package(catkin REQUIRED)

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
# nc_runtime.lib

catkin_package(
  INCLUDE_DIRS include
  CATKIN_DEPENDS
)

file(GLOB_RECURSE SRC "src/*.c" "src/*.cpp" "src/*.h" "include/*.h")
file(GLOB_RECURSE JANSSON "3rd-party/src/jansson/*.cpp" "3rd-party/include/jansson/*.h")

add_library(${PROJECT_NAME} STATIC ${SRC} ${JANSSON})
target_include_directories(${PROJECT_NAME} PRIVATE "src" "include/nc-runtime" "3rd-party/include")
add_definitions(-D_CRTDBG_MAP_ALLOC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "src/stdafx_nc_runtime.h")


########################################################################################
# nc_runtime_test
project(nc_runtime_test)

file(GLOB_RECURSE TEST_SRC "test/*.cpp" "test/*.h")
add_executable(${PROJECT_NAME} ${TEST_SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE "nc_runtime" GTest::gtest)

if (NOT WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE pthread)	# google test needs pthread on Linux
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "src" "include/nc-runtime" "3rd-party/include" "3rd-party/src/googletest-1.10.0")
target_precompile_headers(${PROJECT_NAME} PRIVATE "test/stdafx_nc_runtime_test.h")
