#include <lolbject/Box.h>

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/macros.h>

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

struct LolClass* Box;

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Box);
	return NULL;
}

static struct Box* init_with_value(struct Box* self, va_list arguments)
{
	if (self = lolbj_send_message_to_super(self, "init")) {
		struct Lolbject* caller = va_arg(arguments, struct Lolbject*);
		lolbj_set_object_property(self, "caller", RETAIN(caller));

		void* value = va_arg(arguments, void*);
		self->value = value;
	}

	return self;
}

static struct Lolbject* dealloc_selector(struct Box* self, va_list arguments)
{
	RELEASE(self->caller);
	lolbj_send_message_to_super(self, "dealloc");
	return NULL;
}

void lolbj_box_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, Lolbject);

	lolbj_add_class_selector(klass, "objectSize", object_size_selector);

	lolbj_add_selector(klass, "initWithValue", init_with_value);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);

	lolbj_add_property(klass, "caller", Lolbject, offsetof(struct Box, caller));
}
