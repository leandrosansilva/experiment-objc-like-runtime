set(lolbject_runtime
	runtime.c
	LolRuntime.c
	Lolbject.c
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

target_link_libraries(lolbject_runtime dl)
