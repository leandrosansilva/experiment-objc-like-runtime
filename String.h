#pragma once

#include "object.h"

struct Class_String;

struct String
{
	struct Object super;
	char* content;
};

struct Class_String* String();

void obj_string_initializer(struct Class_String* klass);
