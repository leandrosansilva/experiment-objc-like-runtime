#include <lolbject/Number.h>

#include <lolbject/Class.h>
#include <lolbject/Box.h>
#include <lolbject/runtime.h>

#include <stdlib.h>
#include <stdint.h>

struct Number
{
	struct Lolbject super;
	int value;
};

static struct LolClass _Number;

struct LolClass* Number()
{
	return &_Number;
}

// Selectors
static struct Lolbject* object_size_selector(struct Lolbject* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct Number);
	return NULL;
}

static struct Number* init_with_integer(struct Number* self, va_list arguments)
{
	if (self = obj_send_message_to_super(self, "init")) {
		self->value = va_arg(arguments, int);
	}

	return self;
}

static struct Box* get_boxed_value(struct Number* self, va_list arguments)
{
	return obj_send_message(obj_send_message(Box(), "alloc"), "initWithValue", self, &self->value);
}

void obj_number_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, Lolbject());
	obj_set_class_name(klass, "Number");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "initWithInt", init_with_integer);
	obj_add_selector(klass, "boxedValue", get_boxed_value);
}
