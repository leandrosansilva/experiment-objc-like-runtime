#pragma once

#include "object.h"
#include <stdint.h>

struct Class_Number
{
	struct Class_Object super;
};

struct Number
{
	struct Object super;
	int value;
};

void numberInitializer(struct Class_Number* klass);

struct Class_Number* Number();
