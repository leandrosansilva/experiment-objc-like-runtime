#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

struct Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

struct ObjectSelectorPair;

struct Class_Object
{
	// NOTE: linked list. No need to say how inefficient it is :-)
	// It should be replaced by an array based hash map, but who cares?
	struct ObjectSelectorPair* selectors;

	const char* objectName;
};

struct Object
{
	struct Class_Object *klass;
};

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);

void loadClassObject();

void initializeObject(struct Class_Object* klass);

struct Class_Object* Object_Class_Instance();

void deleteObject(struct Object* object);

void deleteClassSelector(struct Class_Object* klass, struct ObjectSelectorPair* pair);
