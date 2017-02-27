#pragma once

#include "object.h"

struct Class_String
{
	struct Class_Object super;
};

struct String
{
	struct Class_String* klass;
	char* content;
};

void loadClassString();

struct String* allocString();

void initializeString(struct Class_String* klass);

struct Class_String* String_Class_Instance();

void unloadClassString();
