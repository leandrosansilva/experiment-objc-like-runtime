#pragma once

#include <lolbject/api.h>

struct Lolbject_Private;
struct LolClass;

struct Lolbject
{
	struct LolClass* klass;
	struct Lolbject_Private* priv;
};

LOL_EXPORT struct LolClass* Lolbject;

void lolbj_object_initializer(struct LolClass* klass);
