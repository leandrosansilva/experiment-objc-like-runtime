#include <lolbject/Lolbject.h>
#include <lolbject/Class.h>
#include <lolbject/runtime.h>
#include <lolbject/String.h>
#include <lolbject/Number.h>
#include <lolbject/DefaultAllocator.h>
#include <lolbject/macros.h>
#include "Lolbject_Private.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct LolClass* Lolbject;

static struct String* description_selector(struct Lolbject* self, va_list arguments)
{
	struct String* nameAsDescription = STRING(lolbj_class_name(lolbj_class_for_object(self)));
	return nameAsDescription; // TODO: format like: "Lolbject <address>"
}

static struct Lolbject* init_selector(struct Lolbject* self, va_list arguments)
{
	return self;
}

static struct Lolbject* retain_selector(struct Lolbject* self, va_list arguments)
{
	if (!lolbj_object_is_class(self)) {
		self->priv->ref_counter++;
	}

	return self;
}

static struct Lolbject* release_object_selector(struct LolClass* self, va_list arguments)
{
	struct Lolbject** object = va_arg(arguments, struct Lolbject*);

	assert(object != NULL);
	assert(*object != NULL);

	if (lolbj_object_is_class(*object)) {
		return NULL;
	}

	assert((*object)->priv->ref_counter > 0);

	((*object)->priv->ref_counter)--;

	if (((*object)->priv->ref_counter) == 0) {
		lolbj_send_message(*object, "dealloc");
		struct Lolbject* allocator = lolbj_send_message(self, "allocator");
		lolbj_send_message(allocator, "deleteObject", *object);
	} 

	*object = NULL;

	return NULL;
}

static struct Lolbject* get_allocator_selector(struct LolClass* self, va_list arguments)
{
	return DefaultAllocator;
}

static struct Lolbject* alloc_selector(struct LolClass* self, va_list arguments)
{
	struct Lolbject* allocator = lolbj_send_message(self, "allocator");

	size_t size = 0;
	lolbj_send_message(self, "objectSize", &size);

	struct Lolbject* obj = lolbj_send_message(allocator, "allocateObject", size);

	assert(obj);
	assert(obj->priv);

	obj->klass = (struct LolClass*)self;
	obj->priv->tag = lolbj_runtime_type_object;
	obj->priv->ref_counter = 1;

	return obj;
}

static struct Lolbject* dealloc_selector(struct Lolbject* self, va_list arguments)
{
	return NULL;
}

void lolbj_object_initializer(struct LolClass* klass)
{
	lolbj_set_class_parent(klass, NULL);

	// Due some runtime limitations, lolbj_add_class_selector() cannot be used for Object!!!
	lolbj_add_selector(Class, "release", release_object_selector);
	lolbj_add_selector(Class, "alloc", alloc_selector);
	lolbj_add_selector(Class, "allocator", get_allocator_selector);

	lolbj_add_selector(klass, "description", description_selector);
	lolbj_add_selector(klass, "init", init_selector);
	lolbj_add_selector(klass, "dealloc", dealloc_selector);
	lolbj_add_selector(klass, "retain", retain_selector);
}
