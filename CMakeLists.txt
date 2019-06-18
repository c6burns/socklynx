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

include_directories(include)
include_directories(src/socklynx)
add_library(socklynx
	src/socklynx/socklynx.c
	include/socklynx/socklynx.h
	src/socklynx/endpoint.c
	include/socklynx/endpoint.h
	src/socklynx/buf.c
	include/socklynx/buf.h
	src/socklynx/sock.c
	include/socklynx/sock.h
	src/socklynx/sys.c
	include/socklynx/sys.h
	include/socklynx/common.h
	include/socklynx/error.h
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