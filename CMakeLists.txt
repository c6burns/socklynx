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