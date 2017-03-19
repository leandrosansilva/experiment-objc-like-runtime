#pragma once

struct Lolbject_Private;
struct LolClass;

struct Lolbject
{
	struct LolClass* klass;
	struct Lolbject_Private* priv;
};

struct LolClass* Lolbject;

void lolbj_object_initializer(struct LolClass* klass);
