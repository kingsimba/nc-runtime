cmake_minimum_required(VERSION 3.16)
project(nc_runtime C CXX)

include(platform_detect.cmake)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT WIN32)
	add_compile_options(-Wunused-variable -Wunused-parameter -fPIC)
endif()

include_directories("../3rd-party/include")
link_directories("../3rd-party/lib/${NC_PLATFORM}/Release")

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
  LIBRARIES nc_runtime
  CATKIN_DEPENDS
)

file(GLOB_RECURSE SRC "src/*.c" "src/*.cpp" "src/*.h" "include/*.h")
file(GLOB_RECURSE JANSSON "3rd-party/src/jansson/*.cpp" "3rd-party/include/jansson/*.h")

add_library(${PROJECT_NAME} STATIC ${SRC} ${JANSSON})
target_compile_options(${PROJECT_NAME} PRIVATE "-O2")
target_include_directories(${PROJECT_NAME} PRIVATE "src" "include" "3rd-party/include")
add_definitions(-D_CRTDBG_MAP_ALLOC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "src/stdafx_nc_runtime.h")


########################################################################################
# nc_runtime_test
if (NOT ${NC_PLATFORM} MATCHES "pi64")
	project(nc_runtime_test)

	file(GLOB_RECURSE TEST_SRC "test/*.cpp" "test/*.h")
	add_executable(${PROJECT_NAME} ${TEST_SRC})
	target_link_libraries(${PROJECT_NAME} PRIVATE nc_runtime stb GTest::gtest)

	if (NOT WIN32)
		target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
	endif()

	target_include_directories(${PROJECT_NAME} PRIVATE "src" "include" "3rd-party/include")
	target_precompile_headers(${PROJECT_NAME} PRIVATE "test/stdafx_nc_runtime_test.h")
endif()