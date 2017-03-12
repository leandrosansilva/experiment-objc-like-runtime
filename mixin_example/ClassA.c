#include "ClassA.h"

#include <lolbject/Class.h>
#include <lolbject/runtime.h>

#include <stdio.h>

struct ClassA
{
	struct Lolbject super;
};

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassA);
	return NULL;
}

static struct ClassA* hello_selector(struct ClassA* self, va_list arguments)
{
	printf("Hello World from A which is of type '%s'\n", lolbj_class_name(lolbj_class_for_object(self)));
	return self;
}

void lolbj_class_a_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, lolbj_class_with_name(lolbj_core_module(), "Lolbject"));

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "helloFromA", hello_selector);
}
