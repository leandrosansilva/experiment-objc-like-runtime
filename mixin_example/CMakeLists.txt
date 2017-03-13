add_library(
	mixin_example
	SHARED
	ClassA.c
	ClassB.c
	ClassC.c
	ClassD.c
	module.c
)

target_link_libraries(mixin_example lolbject_runtime)
