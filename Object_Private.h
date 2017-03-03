#pragma once

#include <stdint.h>

struct Object_Private
{
	obj_runtime_type tag;
	uint16_t ref_counter;
};
