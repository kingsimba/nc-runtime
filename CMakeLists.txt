cmake_minimum_required(VERSION 3.16)
project(nc_runtime C CXX)

include(platform_detect.cmake)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("../3rd-party/include")
link_directories("../3rd-party/lib/${NC_PLATFORM}/Release")

find_package(catkin REQUIRED)

if (WIN32)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /GF")	# enable string pool
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Ob1")	# inline only __inline
else()
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks")
	add_compile_options(-fPIC)
	add_compile_options(-Wall)
	add_compile_options(-Werror=unused)
	add_compile_options(-Werror=address)
	add_compile_options(-Werror=return-type)
	add_compile_options(-Werror=uninitialized)
	add_compile_options(-Werror=unused-parameter)
	add_compile_options(-Werror=dangling-else)
	add_compile_options(-Wsign-compare)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_options(-Werror=unused-private-field)
		add_compile_options(-Wthread-safety)
	endif()
endif()

########################################################################################
# nc_runtime.lib

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES nc_runtime
  CATKIN_DEPENDS
)

file(GLOB_RECURSE SRC 
./src/nc_string.cpp
./src/nc_argparse.cpp
./src/base64.cpp
./src/nc_array.cpp
./src/nc_file.cpp
./src/nc_log.cpp
./src/turbo_base64/turbob64c.cpp
./src/turbo_base64/turbob64d.cpp
./src/nc_math.cpp
./src/nc_object.cpp
./src/nc_task.cpp
./src/nc_gtest_extension.cpp
./src/stdafx_nc_runtime.cpp
./src/nc_polyline.cpp
./src/nc_image.cpp
./src/string_slice.cpp
./src/nc_event.cpp
./src/quaternion.cpp
./src/nc_stdlib.cpp
./src/costmap_inflator_imple.cpp
./src/json_node.cpp
./src/nc_data.cpp
./src/line_simplifier.cpp
./src/line_parser.cpp
./3rd-party/src/jansson/utf.cpp
)

add_library(${PROJECT_NAME} STATIC ${SRC})
target_compile_options(${PROJECT_NAME} PRIVATE "-O2")
target_include_directories(${PROJECT_NAME} PRIVATE "src" "include" "3rd-party/include")
add_definitions(-D_CRTDBG_MAP_ALLOC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "src/stdafx_nc_runtime.h")


########################################################################################
# nc_runtime_test
if (NOT ${NC_PLATFORM} MATCHES "pi64")
	project(nc_runtime_test)

	file(GLOB_RECURSE TEST_SRC
		./test/nc_event_unittest.cpp
		./test/nc_image_unittest.cpp
		./test/mutex_unittest.cpp
		./test/image_painter_unittest.cpp
		./test/json_node_unittest.cpp
		./test/nc_stdlib_unittest.cpp
		./test/stdafx_nc_runtime_test.cpp
		./test/costmap_inflator_unittest.cpp
		./test/circular_array_unittest.cpp
		./test/nc_string_unittest.cpp
		./test/nc_log_unittest.cpp
		./test/main.cpp
		./test/nc_math_unittest.cpp
		./test/vector3_unittest.cpp
		./test/nc_array_unittest.cpp
		./test/nc_file_unittest.cpp
		./test/nc_object_unittest.cpp
		./test/string_slice_unittest.cpp
		./test/base64_unittest.cpp
		./test/nc_data_unittest.cpp
		./test/quaternion_unittest.cpp
		./test/nc_polyline_unittest.cpp
		./test/dbscan_unittest.cpp
		./test/line_simplifier_unitest.cpp
		./test/nc_task_unittest.cpp
		./test/line_parser_unittest.cpp
	)
	add_executable(${PROJECT_NAME} ${TEST_SRC})
	target_link_libraries(${PROJECT_NAME} PRIVATE nc_runtime stb jansson GTest::gtest)

	if (NOT WIN32)
		target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
	endif()

	target_include_directories(${PROJECT_NAME} PRIVATE "src" "include" "3rd-party/include")
	target_precompile_headers(${PROJECT_NAME} PRIVATE "test/stdafx_nc_runtime_test.h")
endif()
