#include "Number.h"

#include "Class.h"
#include "Box.h"
#include "runtime.h"

#include <stdlib.h>
#include <stdint.h>

struct Class_Number
{
	struct Class_Object super;
};

struct Number
{
	struct Object super;
	int value;
};

static struct Class_Number _Number;

struct Class_Number* Number()
{
	return &_Number;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
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

void obj_number_initializer(struct Class_Number* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "Number");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "initWithInt", init_with_integer);
	obj_add_selector(klass, "boxedValue", get_boxed_value);
}
