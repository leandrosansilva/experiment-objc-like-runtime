#pragma once

#include <lolbject/Object.h>

struct LolClass_Private;

struct LolClass
{
	struct Object proto;
	struct LolClass_Private* priv;
};
