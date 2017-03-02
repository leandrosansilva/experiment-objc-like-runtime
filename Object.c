#include "Object.h"
#include "runtime.h"
#include "String.h"
#include "Number.h"

#include <stdint.h>
#include <assert.h>

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
	printf("Retain Called!\n");

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

	printf("Releasing object with %d counter!\n", (*object)->priv->ref_counter);
	obj_send_message(*object, "dealloc");

	if (--((*object)->priv->ref_counter) == 0) {
		obj_send_message(*object, "dealloc");
		free(*object);
		*object = NULL;
	} else {
		printf("Object still has references, keeping it alive!\n");
	}

	return NULL;
}

static struct Object* alloc_selector(struct Object_Class* self, va_list arguments)
{
	size_t size = 0;

	obj_send_message(self, "objectSize", &size);

	if (size == 0) {
		return NULL;
	}

	assert(size >= sizeof(struct Object) && "Object is too small!");

	struct Object* obj = malloc(size);
	memset(obj, 0, size);

	obj->tag = obj_runtime_type_object;

	obj->klass = (struct Class_Object*)self;

	return obj;
}

static struct Object* dealloc_selector(struct Object* self, va_list arguments)
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
	obj_add_class_selector(klass, "alloc", alloc_selector);

	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
	obj_add_selector(klass, "retain", retain_selector);
}
