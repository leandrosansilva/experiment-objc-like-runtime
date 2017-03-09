set(lolbject_runtime
	runtime.c
	Lolbject.c
)

set(lolbject_core
	String.c
	Number.c
	Box.c
	Array.c
)

add_library(
	lolbject_runtime
	SHARED
	${lolbject_runtime}
)

add_library(
	lolbject_core
	SHARED
	${lolbject_core}
)

target_link_libraries(lolbject_runtime dl)

target_link_libraries(lolbject_core lolbject_runtime)
