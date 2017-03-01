#include "runtime.h"
#include "Object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

static struct Class_Object _Class;

struct Class_Object* Class()
{
	return &_Class;
}

// NOTE: linked list. No need to say how inefficient it is :-)
// It should be replaced by an array based hash map, but who cares?
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

void obj_add_class_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector)
{
	obj_add_selector(klass->proto.klass, selectorName, selector);
}

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName)
{
	// NOTE: this is the worst implementation of method lookup ever!
	for (struct Class_Object* k = klass; k != NULL; k = k->super) {
		for (struct ObjectSelectorPair* pair = k->selectors; pair != NULL; pair = pair->next) {
			if (strcmp(pair->selectorName, selectorName) == 0) {
				return pair->selector;
			}
		}
	}

	return NULL;
}

// FIXME: this function is a copy of obj_send_message. Refactor is needed!
// The problem here is the va_args...
struct Object* obj_send_message_to_super(struct Object* obj, const char* selectorName, ...)
{
	if (obj == NULL) {
		return NULL;
	}

	obj_selector selector = obj_selector_for_name(obj->klass->super, selectorName);

	if (selector == NULL) {
		return NULL;
	}

	va_list arguments;
	va_start(arguments, selectorName);
	struct Object* result = selector(obj, arguments);
	va_end(arguments);

	return result;
}

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...)
{
	if (obj == NULL) {
		return NULL;
	}

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

static struct Object* alloc_selector(struct Object* self, va_list arguments)
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

void obj_class_initializer(struct Class_Object* klass)
{
	klass->objectName = "Class";
	obj_add_selector(klass, "alloc", alloc_selector);
}

static void classStaticInitializer(struct Class_Object* klass)
{
	klass->objectName = NULL;
}

static void privInitializeClass(struct Class_Object* klass, obj_class_initializer_callback initializer, struct Class_Object* super, bool createStatic);

static struct Class_Object* createClassWithStaticMethods()
{
		struct Class_Object* class_with_class_methods = malloc(sizeof(struct Class_Object));
		privInitializeClass(class_with_class_methods, classStaticInitializer, &_Class, false);
		return class_with_class_methods;
}

static void privInitializeClass(struct Class_Object* klass, obj_class_initializer_callback initializer, struct Class_Object* super, bool createStatic)
{
	klass->proto.tag = obj_runtime_type_class;
	klass->proto.klass = createStatic ? createClassWithStaticMethods() : Object();
	klass->super = super;
	klass->selectors = NULL;

	if (initializer != NULL) {
		initializer(klass);
	}
}

void obj_initialize_class(struct Class_Object* klass, obj_class_initializer_callback initializer, struct Class_Object* super)
{
	privInitializeClass(klass, initializer, super, true);
}

static void deleteClassSelector(struct Class_Object* klass, struct ObjectSelectorPair* pair)
{
	struct ObjectSelectorPair* tmp;

	while(pair != NULL) {
		tmp = pair;
		pair = pair->next;
		free(tmp);
	}
}

void obj_unload_class(struct Class_Object* klass)
{
	deleteClassSelector(klass->proto.klass, klass->proto.klass->selectors);
	free(klass->proto.klass);
	deleteClassSelector(klass, klass->selectors);
}
