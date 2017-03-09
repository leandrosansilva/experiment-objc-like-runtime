#pragma once

#include <lolbject/Object.h>

struct Box
{
	struct Object super;
	struct Object* caller;
	void* value;
};

void obj_box_initializer(struct LolClass* klass);

struct LolClass* Box();
