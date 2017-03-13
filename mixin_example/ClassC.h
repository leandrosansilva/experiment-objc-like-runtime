#pragma once

#include <lolbject/Lolbject.h>

struct ClassC
{
	struct Lolbject super;
	struct ClassA* a;
	struct ClassB* b;
};
