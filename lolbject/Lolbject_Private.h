#pragma once

#include <stdint.h>

enum lolbj_runtime_type
{
	lolbj_runtime_type_class = 0x01,
	lolbj_runtime_type_object = 0x02
};

typedef uint8_t lolbj_runtime_type;

struct Lolbject_Private
{
	lolbj_runtime_type tag;
	uint16_t ref_counter;
};
