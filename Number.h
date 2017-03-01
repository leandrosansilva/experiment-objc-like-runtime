#pragma once

#include "object.h"
#include <stdint.h>

struct Class_Number;

struct Number
{
	struct Object super;
	int value;
};

void obj_number_initializer(struct Class_Number* klass);

struct Class_Number* Number();
