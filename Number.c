#include "Number.h"
#include <stdlib.h>

static struct Class_Number _Number_Class_Instance;

struct Class_Number* Number_Class_Instance()
{
	return &_Number_Class_Instance;
}

struct Number* allocNumber()
{
	struct Number* number = allocObject(sizeof(struct Number));
	number->super.klass = &_Number_Class_Instance;
	return number;
}

// Selectors
static struct Number* init_with_integer(struct Number* self, va_list arguments)
{
	self->value = va_arg(arguments, int);
	return self;
}

void numberInitializer(struct Class_Number* klass)
{
	klass->super.objectName = "Number";

	obj_add_selector(klass, "initWithInt", init_with_integer);

	// set selectors here!
}
