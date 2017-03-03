#pragma once

#include <stdint.h>

enum obj_runtime_type
{
	obj_runtime_type_class = 0x01,
	obj_runtime_type_object = 0x02
};

typedef uint8_t obj_runtime_type;

struct Object_Private
{
	obj_runtime_type tag;
	uint16_t ref_counter;
};
