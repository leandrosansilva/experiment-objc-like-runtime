#pragma once

#include "runtime.h"

struct Object;

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

void obj_initialize_class(struct Class_Object* klass, obj_class_initializer_callback initializer, struct Class_Object* super);

struct Class_Object* Object();

void obj_object_initializer(struct Class_Object* klass);
