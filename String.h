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

struct String* allocString();

struct Class_String* String_Class_Instance();

void stringInitializer(struct Class_String* klass);
