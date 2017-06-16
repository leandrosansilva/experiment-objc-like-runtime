#pragma once

#include <lolbject/Lolbject.h>

struct Box
{
	struct Lolbject super;
	struct Lolbject* caller;
	void* value;
};

void lolbj_box_initializer(struct LolRuntime* runtime, struct LolClass* klass);
