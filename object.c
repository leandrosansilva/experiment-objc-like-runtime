#include "object.h"
#include "String.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static struct Class_Object _Object_Class_Instance;

struct Class_Object* Object_Class_Instance()
{
	return &_Object_Class_Instance;
}

struct ObjectSelectorPair
{
	const char* selectorName;
	obj_selector selector;
	struct ObjectSelectorPair* next;
};

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector)
{
	struct ObjectSelectorPair *pair = malloc(sizeof(struct ObjectSelectorPair));
	pair->selectorName = selectorName;
	pair->selector = selector;

	// insert at the beginning of the list
	pair->next = klass->selectors;
	klass->selectors = pair;
}

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName)
{
	for (struct ObjectSelectorPair* pair = klass->selectors; pair != NULL; pair = pair->next) {
		if (strcmp(pair->selectorName, selectorName) == 0) {
			return pair->selector;
		}
	}

	return NULL;
}

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...)
{
	obj_selector selector = obj_selector_for_name(obj->klass, selectorName);

	if (selector == NULL) {
		return NULL;
	}

	va_list arguments;
	va_start(arguments, selectorName);
	struct Object* result = selector(obj, arguments);
	va_end(arguments);

	return result;
}

static struct String* description_selector(struct Object* self, va_list arguments)
{
	struct String* nameAsDescription = obj_send_message(allocString(), "initWithString", self->klass->objectName);
	return nameAsDescription; // TODO: format like: "Object <address>"
}

void initializeClass(struct Class_Object* klass, obj_class_initializer initializer, struct Class_Object* super)
{
	klass->tag = obj_runtime_type_class;
	klass->selectors = NULL;
	klass->objectName = "Object";

	if (super != NULL) {
		// Handle super!!!!
	}

	obj_add_selector(klass, "description", description_selector);

	if (initializer != NULL) {
		initializer(klass);
	}
}

void deleteObject(struct Object* object)
{
	obj_send_message(object, "dealloc");
	free(object);
}

void deleteClassSelector(struct Class_Object* klass, struct ObjectSelectorPair* pair)
{
	struct ObjectSelectorPair* tmp;

	while(pair != NULL) {
		tmp = pair;
		pair = pair->next;
		free(tmp);
	}
}

// wraps malloc
void* allocObject(size_t size)
{
	assert(size >= sizeof(struct Object) && "Object is too small!");
	struct Object* obj = malloc(size);
	obj->tag = obj_runtime_type_object;
	obj->klass = &_Object_Class_Instance;
	return (void*)obj;
}

void unloadClass(struct Class_Object* klass)
{
	deleteClassSelector(klass, klass->selectors);
}
