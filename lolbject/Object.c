#include <lolbject/Object.h>
#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>

#include "Object_Private.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct Class_Object _Object;

struct Class_Object* Object()
{
	return &_Object;
}

static struct String* description_selector(struct Object* self, va_list arguments)
{
	struct String* nameAsDescription = STRING(obj_class_name(obj_class_for_object(self)));
	return nameAsDescription; // TODO: format like: "Object <address>"
}

static struct Object* init_selector(struct Object* self, va_list arguments)
{
	return self;
}

static struct Object* retain_selector(struct Object* self, va_list arguments)
{
	if (!obj_object_is_class(self)) {
		self->priv->ref_counter++;
	}

	return self;
}

static struct Object* release_object_selector(struct Class_Object* self, va_list arguments)
{
	struct Object** object = va_arg(arguments, struct Object*);

	assert(object != NULL);
	assert(*object != NULL);

	if (obj_object_is_class(*object)) {
		return NULL;
	}

	assert((*object)->priv->ref_counter > 0);

	((*object)->priv->ref_counter)--;

	if (((*object)->priv->ref_counter) == 0) {
		obj_send_message(*object, "dealloc");
		free(*object);
	} 

	*object = NULL;

	return NULL;
}

static struct Object* alloc_selector(struct Class_Object* self, va_list arguments)
{
	size_t size = 0;

	obj_send_message(self, "objectSize", &size);

	if (size == 0) {
		return NULL;
	}

	assert(size >= sizeof(struct Object) && "Object is too small!");

	struct Object* obj = malloc(size);
	memset(obj, 0, size);

	obj->klass = (struct Class_Object*)self;

	obj->priv = malloc(sizeof(struct Object_Private));
	obj->priv->tag = obj_runtime_type_object;
	obj->priv->ref_counter = 1;

	return obj;
}

static struct Object* dealloc_selector(struct Object* self, va_list arguments)
{
	free(self->priv);
	return NULL;
}

void obj_object_initializer(struct Class_Object* klass)
{
	obj_set_class_name(klass, "Object");
	obj_set_class_parent(klass, NULL);

	obj_add_class_selector(klass, "release", release_object_selector);
	obj_add_class_selector(klass, "alloc", alloc_selector);

	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
	obj_add_selector(klass, "retain", retain_selector);
}
