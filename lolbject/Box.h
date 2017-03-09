#pragma once

#include <lolbject/Object.h>

struct Class_Box;

struct Box
{
	struct Object super;
	struct Object* caller;
	void* value;
};

void obj_box_initializer(struct Class_Box* klass);

struct Class_Box* Box();
