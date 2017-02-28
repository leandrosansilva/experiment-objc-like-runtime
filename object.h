#pragma once

#include <stdarg.h>
#include <stdlib.h>

struct Object;
struct Class_Object;

typedef struct Object* (*obj_selector)(struct Object*, va_list);

struct ObjectSelectorPair;

enum obj_runtime_type
{
	obj_runtime_type_class = 0x01,
	obj_runtime_type_object = 0x02
};

typedef unsigned char obj_runtime_type;

struct Object
{
	obj_runtime_type tag;
	struct Class_Object* klass;
};

struct Class_Object
{
	struct Object proto;
	struct Class_Object* super;
	const char* objectName;
	struct ObjectSelectorPair* selectors;
};

typedef void (*obj_class_initializer)(struct Class_Object*);

void obj_add_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

void obj_add_class_selector(struct Class_Object* klass, const char* selectorName, obj_selector selector);

obj_selector obj_selector_for_name(struct Class_Object* klass, const char* selectorName);

struct Object* obj_send_message(struct Object* obj, const char* selectorName, ...);

void initializeClass(struct Class_Object* klass, obj_class_initializer initializer, struct Class_Object* super);

struct Class_Object* Object();

struct Class_Object* Class();

void releaseObject(struct Object** object);

void unloadClass(struct Class_Object* klass);

void objectInitializer(struct Class_Object* klass);

void classInitializer(struct Class_Object* klass);
