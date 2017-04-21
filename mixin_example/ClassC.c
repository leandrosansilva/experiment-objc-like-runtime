#include "module.h"
#include "ClassC.h"

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>
#include <lolbject/String.h>

#include <stddef.h>

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassC);
	return NULL;
}

static struct ClassC* init_selector(struct ClassC* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, ClassC, "init")) {
		lolbj_set_object_property(self, "a", lolbj_send_message(lolbj_send_message(ClassA, "alloc"), "init"));
		lolbj_set_object_property(self, "b", lolbj_send_message(lolbj_send_message(ClassB, "alloc"), "init"));
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct ClassC* self, va_list arguments)
{
	RELEASE(self->a);
	RELEASE(self->b);

	return lolbj_send_message_to_super(self, ClassC, "dealloc");
}

void lolbj_class_c_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, lolbj_send_message(lolbj_send_message(LolRuntime, "coreModule"), "classWithName", STRING("Lolbject")));
	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "init", init_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);

	lolbj_add_property(klass, "a", ClassA, offsetof(struct ClassC, a), "helloFromA", NULL);
	lolbj_add_property(klass, "b", ClassB, offsetof(struct ClassC, b), "helloFromB", NULL);
}
