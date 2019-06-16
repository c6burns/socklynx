# Copyright 2019 Chris Burns <chris@kitty.city>
#
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# A copy of the License is located at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# or in the "license" file accompanying this file. This file is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied. See the License for the specific language governing
# permissions and limitations under the License.

cmake_minimum_required(VERSION 3.1)
project(socklynx LANGUAGES C)

include_directories(include)
include_directories(src/socklynx)
add_library(socklynx
	src/socklynx/socklynx.c
	include/socklynx/socklynx.h
	include/socklynx/common.h
	include/socklynx/error.h
	include/socklynx/endpoint.h
	include/socklynx/buf.h
	include/socklynx/sock.h
)

include_directories(src/socklynx_server)
add_executable(socklynx_server
	src/socklynx_server/server.c
	include/socklynx_server/server.h
)
target_link_libraries(socklynx_server socklynx)

include_directories(src/socklynx_loadgen)
add_executable(socklynx_loadgen
	src/socklynx_loadgen/loadgen.c
	include/socklynx_loadgen/loadgen.h
)
target_link_libraries(socklynx_loadgen socklynx)