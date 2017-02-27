#include "Number.h"
#include <stdlib.h>

static struct Class_Number _Number_Class_Instance;

struct Class_Number* Number_Class_Instance()
{
	return &_Number_Class_Instance;
}

void loadClassNumber()
{
}

struct Number* allocNumber()
{
	struct Number* number = malloc(sizeof(struct Number));
	number->klass = &_Number_Class_Instance;
	return number;
}

void unloadClassNumber()
{
	deleteClassSelector(&_Number_Class_Instance, _Number_Class_Instance.super.selectors);
}

// Selectors
static struct Number* init_with_integer(struct Number* self, va_list arguments)
{
	self->value = va_arg(arguments, int);
	return self;
}

void initializeNumber(struct Class_Number* klass)
{
	initializeObject(klass);
	klass->super.objectName = "Number";

	obj_add_selector(klass, "initWithInt", init_with_integer);

	// set selectors here!
}
