#pragma once

#include "object.h"
#include <stdint.h>

struct Class_Number
{
	struct Class_Object super;
};

struct Number
{
	struct Class_Number* klass;
	int value;
};

void loadClassNumber();

struct Number* allocNumber();

void initializeNumber(struct Class_Number* klass);

struct Class_Number* Number_Class_Instance();

void unloadClassNumber();
