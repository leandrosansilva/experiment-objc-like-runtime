#pragma once

#include "runtime.h"

struct Object;
struct Class_Object;

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

void initializeClass(struct Class_Object* klass, obj_class_initializer initializer, struct Class_Object* super);

struct Class_Object* Object();

void objectInitializer(struct Class_Object* klass);

void classInitializer(struct Class_Object* klass);
