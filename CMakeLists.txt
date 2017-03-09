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
	-fno-omit-frame-pointer
	-fno-optimize-sibling-calls # disable tail call elimination
	#-fsanitize=memory
	#-fsanitize=address
	#-fsanitize=undefined
	#-fsanitize=thread
)

#SET(sanitizer_libs
#	ubsan
#	asan
#	lsan
#	)

#set(CMAKE_LINKER "clang-3.9")
#set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_library(
	object_like
	STATIC
	runtime.c
	Object.c
	String.c
	Number.c
	Box.c
	Array.c
	ClassA.c
	ClassB.c
	ClassC.c
	)

add_executable(main main.c)

target_link_libraries(main object_like ${sanitizer_libs})
