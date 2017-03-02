#pragma once

#include "runtime.h"

struct Object;
struct Object_Private;

struct Object
{
	obj_runtime_type tag;
	struct Class_Object* klass;
	struct Object_Private* priv;
};

struct Class_Object
{
	struct Object proto;
	struct Class_Object* parent;
	const char* objectName;
	struct ObjectSelectorPair* selectors;
};

struct Class_Object* Object();

void obj_object_initializer(struct Class_Object* klass);
