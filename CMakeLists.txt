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

include_directories(
	${CMAKE_SOURCE_DIR}
)

#SET(sanitizer_libs
#	ubsan
#	asan
#	lsan
#	)

#set(CMAKE_LINKER "clang-3.9")
#set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

add_subdirectory(lolbject)
add_subdirectory(mixin_example)

add_executable(main main.c)

target_link_libraries(main
	lolbject_core
	${sanitizer_libs}
)
