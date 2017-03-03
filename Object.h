#pragma once

struct Object_Private;
struct Class_Object;

struct Object
{
	struct Class_Object* klass;
	struct Object_Private* priv;
};

struct Class_Object* Object();

void obj_object_initializer(struct Class_Object* klass);
