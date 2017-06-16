#pragma once

#include <lolbject/Lolbject.h>

struct LolClass_Private;

struct LolClass
{
	struct Lolbject proto;
	struct LolClass_Private* priv;
};
