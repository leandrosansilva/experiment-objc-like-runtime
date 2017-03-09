#include <lolbject/Box.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

static struct LolClass _Box;

struct LolClass* Box()
{
	return &_Box;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Box);
	return NULL;
}

static struct Box* init_with_value(struct Box* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		struct Object* caller = va_arg(arguments, struct Object*);
		obj_set_object_property(self, "caller", RETAIN(caller));

		void* value = va_arg(arguments, void*);
		self->value = value;
	}

	return self;
}

static struct Object* dealloc_selector(struct Box* self, va_list arguments)
{
	RELEASE(self->caller);
	obj_send_message_to_super(self, "dealloc");
	return NULL;
}

void obj_box_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "Box");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "initWithValue", init_with_value);
	obj_add_selector(klass, "dealloc", dealloc_selector);

	obj_add_property(klass, "caller", Object(), offsetof(struct Box, caller));
}
