#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>
#include <lolbject/String.h>

#include <stdio.h>

struct ClassB
{
	struct Lolbject super;
};

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassB);
	return NULL;
}

static struct ClassB* hello_selector(struct ClassB* self, va_list arguments)
{
	printf("Hello World from B which is of type '%s'\n", lolbj_class_name(lolbj_class_for_object(self)));
	return self;
}

void lolbj_class_b_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, lolbj_send_message(lolbj_send_message(LolRuntime, "coreModule"), "classWithName", STRING("Lolbject")));
	lolbj_set_class_name(klass, "ClassB");

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "helloFromB", hello_selector);
}
