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

set(SL_SRCS
	src/socklynx/socklynx.c
	include/socklynx/socklynx.h
	include/socklynx/endpoint.h
	include/socklynx/buf.h
	include/socklynx/sock.h
	include/socklynx/sys.h
	include/socklynx/common.h
	include/socklynx/error.h
)
include_directories(include)
include_directories(src/socklynx)
add_library(socklynx MODULE ${SL_SRCS})
if(WIN32)
	target_link_libraries(socklynx Ws2_32.lib)
endif()

#include_directories(src/socklynx_server)
#add_executable(socklynx_server
#	src/socklynx_server/server.c
#	include/socklynx_server/server.h
#)
#target_link_libraries(socklynx_server socklynx)

#include_directories(src/socklynx_loadgen)
#add_executable(socklynx_loadgen
#	src/socklynx_loadgen/loadgen.c
#	include/socklynx_loadgen/loadgen.h
#)
#target_link_libraries(socklynx_loadgen socklynx)

if(NOT APPLE)
	add_custom_command(
		TARGET socklynx
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:socklynx>
			${CMAKE_SOURCE_DIR}/UnityProject/Assets/SockLynx/Plugins/x86_64/$<TARGET_FILE_NAME:socklynx>
	)
else()
	set_target_properties(socklynx PROPERTIES BUNDLE TRUE)
	add_custom_command(
		TARGET socklynx
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
			$<TARGET_FILE:socklynx>
			${CMAKE_SOURCE_DIR}/UnityProject/Assets/SockLynx/Plugins/x86_64/$<TARGET_FILE_NAME:socklynx>.bundle
	)
endif()