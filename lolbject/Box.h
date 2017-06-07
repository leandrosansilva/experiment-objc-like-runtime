#pragma once

#include <lolbject/Lolbject.h>

struct Box
{
	struct Lolbject super;
	struct Lolbject* caller;
	void* value;
};

void lolbj_box_initializer(struct LolClass* klass);

LOL_EXPORT struct LolClass* Box;
