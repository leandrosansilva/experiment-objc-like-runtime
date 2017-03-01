#include "Object.h"
#include "runtime.h"
#include "String.h"
#include "Number.h"
#include <stdio.h>

static struct Class_Object _Object;

struct Object_Private
{
	int something;
};

struct Class_Object* Object()
{
	return &_Object;
}

static struct String* description_selector(struct Object* self, va_list arguments)
{
	struct String* nameAsDescription = obj_send_message(obj_send_message(String(), "alloc"), "initWithString", self->klass->objectName);
	return nameAsDescription; // TODO: format like: "Object <address>"
}

static struct String* init_selector(struct Object* self, va_list arguments)
{
	printf("Calling Object::init\n");
	self->priv = malloc(sizeof(struct Object_Private));
	return NULL;
}

static struct String* dealloc_selector(struct Object* self, va_list arguments)
{
	printf("Calling Object::dealloc\n");
	if (self) {
		free(self->priv);
	}
	return NULL;
}

void obj_object_initializer(struct Class_Object* klass)
{
	klass->objectName = "Object";
	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
}
