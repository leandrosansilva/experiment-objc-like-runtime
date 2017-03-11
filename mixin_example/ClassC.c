#include "ClassC.h"

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

#include <stddef.h>

struct ClassC
{
	struct Lolbject super;
	struct ClassA* a;
	struct ClassB* b;
};

static struct Lol_Class* ClassA;
static struct Lol_Class* ClassB;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassC);
	return NULL;
}

static struct ClassC* init_selector(struct ClassC* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		obj_set_object_property(self, "a", obj_send_message(obj_send_message(ClassA, "alloc"), "init"));
		obj_set_object_property(self, "b", obj_send_message(obj_send_message(ClassB, "alloc"), "init"));
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct ClassC* self, va_list arguments)
{
	RELEASE(self->a);
	RELEASE(self->b);

	obj_send_message_to_super(self, "dealloc");

	return self;
}

void obj_class_c_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, obj_class_with_name("Lolbject"));

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);

	// TODO: ask to the module, not to the runtime!
	// as those classes might not yet been registred
	ClassA = obj_class_with_name("ClassA");
	ClassB = obj_class_with_name("ClassB");

	obj_add_property(klass, "a", ClassA, offsetof(struct ClassC, a));
	obj_add_property(klass, "b", ClassB, offsetof(struct ClassC, b));

	obj_add_selector_from_property(klass, ClassA, "a", "helloFromA");
	obj_add_selector_from_property(klass, ClassB, "b", "helloFromB");
}
