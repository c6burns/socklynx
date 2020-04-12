#
# Copyright (c) 2019 Chris Burns <chris@kitty.city>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

cmake_minimum_required(VERSION 3.1)
project(socklynx LANGUAGES C)
include(CMakeDependentOption)

set(SL_STATIC_BUILD OFF CACHE BOOL "Build as a static library as opposed to as a plugin")
CMAKE_DEPENDENT_OPTION(SL_SKIP_POSTBUILD "Copy the DSO into UnityProject" OFF "SL_STATIC_BUILD" ON)
set(SL_DEPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(SL_LIBRARIES "")
set(SL_INCLUDE_DIRS "")

if(NOT EXISTS ${SL_DEPS_DIR})
	file(MAKE_DIRECTORY ${SL_DEPS_DIR})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	list(APPEND SL_TARGET_COMPILE_DEFS -DSL_CMAKE_BUILD_DEBUG)
endif()

if(NOT WIN32)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c99")
	if(NOT APPLE)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")
	endif()
else()
	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
	)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()

	if(MSVC)
		add_custom_target(CMake SOURCES CMakeLists.txt)
		list(APPEND SL_TARGET_COMPILE_DEFS -D_CRT_SECURE_NO_WARNINGS)
	endif()
endif()

#
# aws-c-common dependency
set(BUILD_TESTING ON CACHE BOOL "" FORCE)
set(STATIC_CRT ON CACHE BOOL "" FORCE)
mark_as_advanced(FORCE BUILD_TESTING STATIC_CRT)

if(NOT EXISTS ${SL_DEPS_DIR}/aws-c-common)
	execute_process(COMMAND git clone https://github.com/c6burns/aws-c-common WORKING_DIRECTORY ${SL_DEPS_DIR})
endif()

add_custom_target(aws-c-common_update
	COMMAND git checkout master
	COMMAND git pull
	WORKING_DIRECTORY ${SL_DEPS_DIR}/aws-c-common
)

set(ORIG_CMAKE_PROJECT_NAME ${CMAKE_PROJECT_NAME})
set(CMAKE_PROJECT_NAME aws-c-common)
list(APPEND SL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/deps/aws-c-common/include)
list(APPEND SL_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/deps/aws-c-common/generated/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/deps/aws-c-common/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/deps/aws-c-common/generated/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/aws-c-common)
add_dependencies(aws-c-common aws-c-common_update)
list(APPEND SL_LIBRARIES aws-c-common)
set(CMAKE_PROJECT_NAME ${ORIG_CMAKE_PROJECT_NAME})

set(SL_SKIP_POSTBUILD false CACHE BOOL "Copy the DSO into UnityProject")

#
# build socklynx library
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/aws-c-common/cmake)
include(slTesting)
enable_testing()

set(SL_SRC ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND SL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND SL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/socklynx)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src/socklynx)
set(SL_LIBRARY_SRCS
	src/socklynx/socklynx.c
	include/socklynx/socklynx.h
	include/socklynx/endpoint.h
	include/socklynx/buf.h
	include/socklynx/sock.h
	include/socklynx/sys.h
	include/socklynx/common.h
	include/socklynx/test_harness.h
	include/socklynx/error.h
)
if(SL_STATIC_BUILD)
    set(SL_LIBRARY_SRCS ${SL_LIBRARY_SRCS} ${SL_DSO_SRCS})
else()
    add_library(socklynxDSO MODULE ${SL_DSO_SRCS})
endif()

add_library(socklynx STATIC ${SL_LIBRARY_SRCS})
target_link_libraries(socklynx ${SL_LIBRARIES})
target_compile_definitions(socklynx PRIVATE ${SL_TARGET_COMPILE_DEFS})
target_compile_definitions(socklynx PRIVATE -DSL_EXPORTING)
list(APPEND SL_LIBRARIES socklynx)

if(WIN32)
    target_link_libraries(socklynx Ws2_32.lib)
    if(NOT SL_STATIC_BUILD)
        target_link_libraries(socklynxDSO Ws2_32.lib)
    endif()
endif()

#
# build socklynx test executables
set(SL_TESTS_C
	"tests/test_api.c"
	"tests/test_integration.c"
)
set(TESTS ${SL_TESTS_C})

sl_add_test_case(sl_udp_setup)
sl_add_test_case(sl_udp_doublesetup)
sl_add_test_case(sl_udp_cleanup)
sl_add_test_case(sl_udp_doublecleanup)
sl_add_test_case(sl_udp_setupcleanup)
sl_add_test_case(sl_udp_doublesetupcleanup)
sl_add_test_case(sl_udp_newsocket)
sl_add_test_case(sl_udp_socketopenclose)
sl_add_test_case(sl_udp_socketsetblocking)
sl_add_test_case(sl_udp_socketsendrecv_blocking)

sl_generate_test_driver(sl-tests sl)
target_link_libraries(sl-tests ${SL_LIBRARIES})
target_link_libraries(sl-tests socklynx)
target_compile_definitions(sl-tests PRIVATE ${SL_TARGET_COMPILE_DEFS})

# runner for single test for easy debug step through an impl issue
#sl_test_single(tests/test_api sl_udp_socketsetblocking "${SL_LIBRARIES}" "${SL_TARGET_COMPILE_DEFS}")
#sl_test_single(test_integration sl_udp_socketsendrecv_blocking "${SL_LIBRARIES}" "${SL_TARGET_COMPILE_DEFS}")

#include_directories(src/socklynx_server)
#add_executable(socklynx_server
#	src/socklynx_server/server.c
#	include/socklynx_server/server.h
#)
#target_link_libraries(socklynx_server socklynxDSO)

#include_directories(src/socklynx_loadgen)
#add_executable(socklynx_loadgen
#	src/socklynx_loadgen/loadgen.c
#	include/socklynx_loadgen/loadgen.h
#)
#target_link_libraries(socklynx_loadgen socklynxDSO)

if(SL_SKIP_POSTBUILD)
	if(NOT APPLE)
		add_custom_command(
			TARGET socklynxDSO
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
				$<TARGET_FILE:socklynxDSO>
				${CMAKE_SOURCE_DIR}/UnityProject/Assets/SockLynx/Plugins/x86_64/$<TARGET_FILE_NAME:socklynxDSO>
		)
	else()
		set_target_properties(socklynxDSO PROPERTIES BUNDLE TRUE)
	
		# copy dylib for .net core
		add_custom_command(
			TARGET socklynxDSO
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
				$<TARGET_FILE:socklynxDSO>
				${CMAKE_SOURCE_DIR}/UnityProject/Assets/SockLynx/Plugins/x86_64/$<TARGET_FILE_NAME:socklynxDSO>.dylib
		)

		# copy bundle for unity
		add_custom_command(
			TARGET socklynxDSO
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
				$<TARGET_FILE:socklynxDSO>
				${CMAKE_SOURCE_DIR}/UnityProject/Assets/SockLynx/Plugins/x86_64/$<TARGET_FILE_NAME:socklynxDSO>.bundle
		)
	endif()
endif()
