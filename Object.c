#include "Object.h"
#include "runtime.h"
#include "String.h"
#include "Number.h"

#include <stdint.h>

static struct Class_Object _Object;

struct Object_Private
{
	uint16_t ref_counter;
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

static struct Object* init_selector(struct Object* self, va_list arguments)
{
	self->priv = malloc(sizeof(struct Object_Private));
	self->priv->ref_counter = 1;
	return self;
}

static struct Object* retain_selector(struct Object* self, va_list arguments)
{
	if (self != NULL) {
		self->priv->ref_counter++;
	}

	return self;
}

static struct Object* release_object_selector(struct Object* self, va_list arguments)
{
	struct Object** object = va_arg(arguments, struct Object*);

	if (object == NULL) {
		return NULL;
	}

	if (*object == NULL) {
		return NULL;
	}

	obj_send_message(*object, "dealloc");
	free(*object);

	*object = NULL;

	return NULL;
}

static struct String* dealloc_selector(struct Object* self, va_list arguments)
{
	if (self) {
		free(self->priv);
	}
	return NULL;
}

void obj_object_initializer(struct Class_Object* klass)
{
	klass->objectName = "Object";
	klass->parent = NULL;

	obj_add_class_selector(klass, "release", release_object_selector);

	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
	obj_add_selector(klass, "retain", retain_selector);
}
