#pragma once

#include "Object.h"

struct Class_Box;

struct Box
{
	struct Object super;
	void* value;
};

void obj_box_initializer(struct Class_Box* klass);

struct Class_Box* Box();
