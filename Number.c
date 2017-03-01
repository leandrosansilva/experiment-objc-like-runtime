#include "Number.h"
#include <stdlib.h>

struct Class_Number
{
	struct Class_Object super;
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

void obj_number_initializer(struct Class_Number* klass)
{
	klass->super.objectName = "Number";

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "initWithInt", init_with_integer);
}
