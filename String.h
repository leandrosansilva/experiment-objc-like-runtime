#pragma once

#include "object.h"

struct Class_String
{
	struct Class_Object super;
};

struct String
{
	struct Object super;
	char* content;
};

struct Class_String* String();

void stringInitializer(struct Class_String* klass);
