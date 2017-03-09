#include "ClassA.h"

#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/Object.h>

#include <stdio.h>

struct ClassA
{
	struct Object super;
};

static struct Class_Object _ClassA;

struct Class_ClassA* ClassA()
{
	return &_ClassA;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassA);
	return NULL;
}

static struct ClassA* hello_selector(struct ClassA* self, va_list arguments)
{
	printf("Hello World from A which is of type '%s'\n", obj_class_name(obj_class_for_object(self)));
	return self;
}

void obj_class_a_initializer(struct Class_ClassA* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "ClassA");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "helloFromA", hello_selector);
}
