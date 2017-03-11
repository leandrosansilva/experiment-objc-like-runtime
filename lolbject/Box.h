#pragma once

#include <lolbject/Lolbject.h>

struct Box
{
	struct Lolbject super;
	struct Lolbject* caller;
	void* value;
};

void obj_box_initializer(struct LolClass* klass);

extern struct LolClass* Box;
