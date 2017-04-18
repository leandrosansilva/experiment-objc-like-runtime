set(lolbject_runtime
	runtime.c
	Lolbject.c
	String.c
	Number.c
	Box.c
	Array.c
	MutableArray.c
	DefaultAllocator.c
	TreeObject.c
	SignalSender.c
	Anonymous.c
)

add_library(
	lolbject_runtime
	SHARED
	${lolbject_runtime}
)

target_link_libraries(lolbject_runtime dl)
