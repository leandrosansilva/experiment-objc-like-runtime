project(objc-like)

cmake_minimum_required(VERSION 2.8)

add_definitions(
	-Wall
	-Wextra
	-Wno-parentheses
	-Wno-unused-parameter
	-Wno-incompatible-pointer-types
	-Werror=return-type
	-std=c99
)

add_library(
	object_like
	SHARED
	runtime.c
	Object.c
	String.c
	Number.c
	)

add_executable(main main.c)

target_link_libraries(main object_like)
