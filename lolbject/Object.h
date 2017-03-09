#pragma once

struct Object_Private;
struct LolClass;

struct Object
{
	struct LolClass* klass;
	struct Object_Private* priv;
};

struct LolClass* Object();

void obj_object_initializer(struct LolClass* klass);
