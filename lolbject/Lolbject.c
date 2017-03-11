#include <lolbject/Lolbject.h>
#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/macros.h>

#include "Lolbject_Private.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct LolClass* Lolbject;

static struct String* description_selector(struct Lolbject* self, va_list arguments)
{
	struct String* nameAsDescription = STRING(obj_class_name(obj_class_for_object(self)));
	return nameAsDescription; // TODO: format like: "Lolbject <address>"
}

static struct Lolbject* init_selector(struct Lolbject* self, va_list arguments)
{
	return self;
}

static struct Lolbject* retain_selector(struct Lolbject* self, va_list arguments)
{
	if (!obj_object_is_class(self)) {
		self->priv->ref_counter++;
	}

	return self;
}

static struct Lolbject* release_object_selector(struct LolClass* self, va_list arguments)
{
	struct Lolbject** object = va_arg(arguments, struct Lolbject*);

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

static struct Lolbject* alloc_selector(struct LolClass* self, va_list arguments)
{
	size_t size = 0;

	obj_send_message(self, "objectSize", &size);

	if (size == 0) {
		return NULL;
	}

	assert(size >= sizeof(struct Lolbject) && "Lolbject is too small!");

	struct Lolbject* obj = malloc(size);
	memset(obj, 0, size);

	obj->klass = (struct LolClass*)self;

	obj->priv = malloc(sizeof(struct Lolbject_Private));
	obj->priv->tag = obj_runtime_type_object;
	obj->priv->ref_counter = 1;

	return obj;
}

static struct Lolbject* dealloc_selector(struct Lolbject* self, va_list arguments)
{
	free(self->priv);
	return NULL;
}

void obj_object_initializer(struct LolClass* klass)
{
	obj_set_class_parent(klass, NULL);

	// Due some runtime limitations, obj_add_class_selector() cannot be used for Object!!!
	obj_add_selector(Class, "release", release_object_selector);
	obj_add_selector(Class, "alloc", alloc_selector);

	obj_add_selector(klass, "description", description_selector);
	obj_add_selector(klass, "init", init_selector);
	obj_add_selector(klass, "dealloc", dealloc_selector);
	obj_add_selector(klass, "retain", retain_selector);
}
