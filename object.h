#pragma once

#include <stdarg.h>
#include <stdlib.h>

struct Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

struct ObjectSelectorPair;

enum obj_runtime_type
{
	obj_runtime_type_class = 0x01,
	obj_runtime_type_object = 0x02
};

typedef unsigned char obj_runtime_type;

struct Class_Object
{
	obj_runtime_type tag;

	// NOTE: linked list. No need to say how inefficient it is :-)
	// It should be replaced by an array based hash map, but who cares?
	struct ObjectSelectorPair* selectors;

	const char* objectName;
};

struct Object
{
	obj_runtime_type tag;
	struct Class_Object *klass;
};

typedef void (*obj_class_initializer)(struct Class_Object*);

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);

void initializeClass(struct Class_Object* klass, obj_class_initializer initializer, struct Class_Object* super);

struct Class_Object* Object_Class_Instance();

void deleteObject(struct Object* object);

void deleteClassSelector(struct Class_Object* klass, struct ObjectSelectorPair* pair);

void* allocObject(size_t size);

void unloadClass(struct Class_Object* klass);
