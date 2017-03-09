#include "ClassC.h"

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Object.h>

#include "ClassA.h"
#include "ClassB.h"

#include <stddef.h>

struct ClassC
{
	struct Object super;
	struct ClassA* a;
	struct ClassB* b;
};

static struct Class_Object _ClassC;

struct Class_ClassC* ClassC()
{
	return &_ClassC;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassC);
	return NULL;
}

static struct ClassC* init_selector(struct ClassC* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		self->a = obj_send_message(obj_send_message(ClassA(), "alloc"), "init");
		self->b = obj_send_message(obj_send_message(ClassB(), "alloc"), "init");
	}

	return self;
}

static struct Object* dealloc_selector(struct ClassC* self, va_list arguments)
{
	RELEASE(self->a);
	RELEASE(self->b);

	obj_send_message_to_super(self, "dealloc");

	return self;
}

void obj_class_c_initializer(struct Class_ClassC* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "ClassC");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);

	obj_add_property(klass, "a", ClassA(), offsetof(struct ClassC, a));
	obj_add_property(klass, "b", ClassB(), offsetof(struct ClassC, b));

	obj_add_selector_from_property(klass, ClassA(), "a", "helloFromA");
	obj_add_selector_from_property(klass, ClassB(), "b", "helloFromB");
}
