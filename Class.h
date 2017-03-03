#pragma once

#include "Object.h"

struct Class_Object_Private;

struct Class_Object
{
	struct Object proto;
	struct Class_Object_Private* priv;
};
