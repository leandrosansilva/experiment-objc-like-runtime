#include "ClassB.h"

#include "Class.h"
#include "runtime.h"
#include "Object.h"

#include <stdio.h>

struct ClassB
{
	struct Object super;
};

static struct Class_Object _ClassB;

struct Class_ClassB* ClassB()
{
	return &_ClassB;
}

// Selectors
static struct Object* object_size_selector(struct Object* self, va_list arguments)
{
	size_t* size = va_arg(arguments, size_t*);
	*size = sizeof(struct ClassB);
	return NULL;
}

static struct ClassB* hello_selector(struct ClassB* self, va_list arguments)
{
	printf("Hello World from B which is of type '%s'\n", obj_class_name(obj_class_for_object(self)));
	return self;
}

void obj_class_b_initializer(struct Class_ClassB* klass)
{
	obj_set_class_parent(klass, Object());
	obj_set_class_name(klass, "ClassB");

	obj_add_class_selector(klass, "objectSize", object_size_selector);

	obj_add_selector(klass, "helloFromB", hello_selector);
}
